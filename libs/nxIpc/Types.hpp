#pragma once
#include <switch.h>
#include <cstring>
#include <algorithm>
#include "Exceptions.hpp"

namespace nxIpc
{
	struct Buffer
	{
		Buffer(const void* data, size_t len) : data(data), length(len) {}

		const void* data;
		size_t length;

		inline void CopyTo(void* src)
		{
			std::memcpy(src, data, length);
		}

		inline void CopyTo_s(void* src, size_t size)
		{
			std::memcpy(src, data, std::min(size, length));
		}
	};

	struct WritableBuffer
	{
		WritableBuffer(void* data, size_t len) : data(data), length(len) {}

		void* data;
		size_t length;

		inline void AssignFrom(const void* src)
		{
			std::memcpy(data, src, length);
		}

		inline void AssignFrom_s(const void* src, size_t size)
		{
			std::memcpy(data, src, std::min(size, length));
		}
	};

	// Warning about TLS usage:
	//   This class doesn't copy the TLS data but works with it in-place, this means that any
	//   call that invokes an IPC request (either manually or from libnx like fs access)
	//   will overwrite the TLS with the response from that call, either do it in another thread
	//   or get all the data you need out of the Request class first
	struct Request
	{
		Request(const Request&) = delete;
		Request& operator=(const Request&) = delete;

		HipcParsedRequest hipc;
		u64 cmdId = 0;
		u32 size = 0;
		const void* ptr = nullptr;

		template <typename T> const T* Payload()
		{
			AssertTLSValid();

			if (sizeof(T) > size)
				throw std::logic_error("Payload size is wrong");

			return (T*)ptr;
		}

		Buffer ReadBuffer(u8 index)
		{
			if (index >= hipc.meta.num_send_buffers)
				throw std::logic_error("Read buffer index out of bounds");

			Buffer res = { hipcGetBufferAddress(&sendBuffer), hipcGetBufferSize(&sendBuffer) };

			if (!res.data)
				throw std::runtime_error("Read buffer pointer is null");

			return res;
		}

		WritableBuffer WriteBuffer(u8 index)
		{
			if (index >= hipc.meta.num_recv_buffers)
				throw std::logic_error("Write buffer index out of bounds");

			WritableBuffer res = { hipcGetBufferAddress(&recvBuffer), hipcGetBufferSize(&recvBuffer) };

			if (!res.data)
				throw std::runtime_error("Write buffer pointer is null");

			return res;
		}

		static Request ParseFromTLS()
		{
			return Request();
		}

	private:
		Request()
		{
			void* base = armGetTls();
			hipc = hipcParseRequest(base);

			if (hipc.meta.type == CmifCommandType_Request)
			{
				CmifInHeader* header = (CmifInHeader*)cmifGetAlignedDataStart(hipc.data.data_words, base);
				size_t dataSize = hipc.meta.num_data_words * 4;

				if (!header)
					throw std::runtime_error("HeaderPTR is null");
				if (dataSize < sizeof(CmifInHeader))
					throw std::runtime_error("Data size is smaller than sizeof(IPCServerHeader)");
				if (header->magic != CMIF_IN_HEADER_MAGIC)
					throw std::runtime_error("Header magic is wrong");

				cmdId = header->command_id;
				size = dataSize - sizeof(CmifInHeader);
				if (size)
					ptr = (u8*)(header + 1);

				//Backup buffers to prevent TLS issues
				if (hipc.meta.num_send_buffers)
					sendBuffer = hipc.data.send_buffers[0];
				if (hipc.meta.num_recv_buffers)
					recvBuffer = hipc.data.recv_buffers[0];
			}
		}

		void AssertTLSValid() 
		{
			if (!ptr) return;

			auto header = (CmifInHeader*)ptr - 1;

			//Not the best way but is enough to prevent accidental IPC calls 
			if (header->magic != CMIF_IN_HEADER_MAGIC)
				throw std::runtime_error("Header asserition failed, TLS has been modified since this Request object has been created");
		}

		HipcBufferDescriptor sendBuffer;
		HipcBufferDescriptor recvBuffer;
	};

	struct Response
	{
		template<typename T>
		Response& Payload(const T& p)
		{
			if (R_FAILED(result))
				throw std::logic_error("Payload is not supported when result is non zero");

			payload = { &p, sizeof(T) };
			return *this;
		}

		Response(Result rc = 0)
		{
			result = rc;
		}

		Response& CopyHandle(Handle h)
		{
			if (R_FAILED(result))
				throw std::logic_error("handles are not supported when result is non zero");

			meta.num_copy_handles = 1;
			copyHandle = h;
			return *this;
		}

		Result Finalize()
		{
			meta.type = CmifCommandType_Request;
			meta.num_data_words = (sizeof(CmifOutHeader) + payload.length + 0x10) / 4;

			void* base = armGetTls();
			HipcRequest hipc = hipcMakeRequest(base, meta);
			CmifOutHeader* rawHeader = (CmifOutHeader*)cmifGetAlignedDataStart(hipc.data_words, base);

			rawHeader->magic = CMIF_OUT_HEADER_MAGIC;
			rawHeader->result = result;
			rawHeader->token = 0;

			if (payload.length)
				std::memcpy(((u8*)rawHeader) + sizeof(CmifOutHeader), payload.data, payload.length);

			if (meta.num_copy_handles)
				hipc.copy_handles[0] = copyHandle;

			return result;
		}

	protected:
		HipcMetadata meta = { 0 };

		Buffer payload = { 0, 0 };
		Handle copyHandle;

		Result result;
	};
}