#pragma once
#include <mutex>

//constexpr size_t MAX_BUFFER_SIZE = 4096000;
constexpr size_t MAX_BUFFER_SIZE = 100;
constexpr size_t LAST_BUFFER_INDEX = MAX_BUFFER_SIZE - 1;

// ring buffer options
enum class rbuf_opt_e
{
	RBUF_CLEAR = 0,
	RBUF_NO_CLEAR,
};

// buffer messages
enum class rbuf_msg_e
{
	RBUF_EMPTY = -1,
	RBUF_FULL
};

class RingBuffer
{
public:
	void Init()
	{
		memset(mBuffer, 0, sizeof(mBuffer));
		mHead = 0;
		mTail = 0;
		mSize = 0;
	}
	bool IsEmpty()
	{
		return 0 == mSize;
	}
	bool IsFull()
	{
		return MAX_BUFFER_SIZE == mSize;
	}

	size_t GetSize()
	{
		return mSize;
	}

	bool PutData(const void* data, size_t size)
	{
		bool bResult = false;
		do
		{
			if (size == 0)
				break;

			if (size > MAX_BUFFER_SIZE)
				break;

			if (size + mSize > MAX_BUFFER_SIZE)
				break;

			char* dataPos = (char*)data;
			unsigned __int64 curTailBegin = mTail;
			unsigned __int64 nextTail = mTail + (unsigned __int64)size;
			unsigned __int64 curTailEnd = mTail + (unsigned __int64)size - 1;

			bool bOver = false;
			if (nextTail > LAST_BUFFER_INDEX)
			{
				nextTail = nextTail - MAX_BUFFER_SIZE;
				bOver = true;
			}
			if (mHead != mTail)
			{
				if (bOver)
				{
					if ((curTailBegin <= mHead && mHead < MAX_BUFFER_SIZE)
						|| (0 <= mHead && mHead < nextTail))
						break;
				}
				else
				{
					if (curTailBegin <= mHead && mHead < nextTail)
						break;
				}
			}
			if (curTailBegin < MAX_BUFFER_SIZE && curTailEnd < MAX_BUFFER_SIZE && curTailBegin <= curTailEnd)
			{
				memcpy(&mBuffer[curTailBegin], data, size);
			}
			else
			{
				size_t firstSize = MAX_BUFFER_SIZE - curTailBegin;
				size_t SecondSize = size - firstSize;

				memcpy(&mBuffer[curTailBegin], &dataPos[0], firstSize);
				memcpy(&mBuffer[0], &dataPos[firstSize], SecondSize);
			}

			mTail = nextTail;
			mSize += size;
			bResult = true;
		} while (false);
		return bResult;
	}
	size_t GetData(void* data, size_t size, rbuf_opt_e clear = rbuf_opt_e::RBUF_CLEAR)
	{
		size_t szResult = 0;
		do
		{
			if (size == 0)
				break;

			if (size > MAX_BUFFER_SIZE)
				break;

			if (size > mSize)
				break;

			char* dataPos = (char*)data;
			unsigned __int64 curHeadBegin = mHead;
			unsigned __int64 nextHead = mHead + (unsigned __int64)size;
			unsigned __int64 curHeadEnd = mHead + (unsigned __int64)size - 1;
			bool bOver = false;

			if (nextHead > LAST_BUFFER_INDEX)
			{
				nextHead = nextHead - MAX_BUFFER_SIZE;
				bOver = true;
			}
			if (mHead != mTail)
			{
				if (bOver)
				{
					if ((curHeadBegin <= mTail && mTail < MAX_BUFFER_SIZE)
						|| (0 <= mTail && mTail < nextHead))
						break;
				}
				else
				{
					if (curHeadBegin <= mTail && mTail < nextHead)
						break;
				}
			}
			if (curHeadBegin < MAX_BUFFER_SIZE && curHeadEnd < MAX_BUFFER_SIZE && curHeadBegin <= curHeadEnd)
			{
				memcpy(dataPos, &mBuffer[curHeadBegin], size);
			}
			else
			{
				size_t firstSize = MAX_BUFFER_SIZE - curHeadBegin;
				size_t SecondSize = size - firstSize;

				memcpy(&dataPos[0], &mBuffer[curHeadBegin], firstSize);
				memcpy(&dataPos[firstSize], &mBuffer[0], SecondSize);
			}
			if (clear == rbuf_opt_e::RBUF_CLEAR)
			{
				mHead = nextHead;
				mSize -= size;
			}
			szResult = size;
		} while (false);
		return szResult;
	}

	bool DeleteData(size_t size)
	{
		bool bResult = false;
		do
		{
			if (size == 0)
				break;

			if (size >
				MAX_BUFFER_SIZE)
				break;

			if (size > mSize)
				break;

			unsigned __int64 curHead = mHead;
			unsigned __int64 nextHead = mHead + (unsigned __int64)size;
			if (nextHead > LAST_BUFFER_INDEX)
			{
				nextHead = nextHead - MAX_BUFFER_SIZE;
			}

			mHead = nextHead;
			mSize -= size;

			bResult = true;
		} while (false);
		return bResult;
	}

	void Flush(rbuf_opt_e clear)
	{
		mSize = 0;
		mHead = 0;
		mTail = 0;

		if (rbuf_opt_e::RBUF_CLEAR == clear)
		{
			memset(mBuffer, 0, sizeof(mBuffer));
		}
	}

	void Clear()
	{
		mSize = 0;
		mHead = 0;
		mTail = 0;
	}
	size_t GetMaxSize()
	{
		return MAX_BUFFER_SIZE;
	}

private:
	char mBuffer[MAX_BUFFER_SIZE] = { 0, };
	unsigned __int64 mHead = 0;
	unsigned __int64 mTail = 0;
	size_t mSize = 0;
};
