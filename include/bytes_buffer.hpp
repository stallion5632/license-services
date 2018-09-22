

#ifndef ByteBuffer_Buffer_H
#define ByteBuffer_Buffer_H

#include <iostream>
#include <algorithm>
#include <vector>
#include <assert.h>

// 字符流缓存和管理
// ref:https://github.com/chenshuo/muduo/blob/master/muduo/net/Buffer.h
//	《Linux多线程服务端编程-使用muduo C++网络库》7.4节
// @code
// +-------------------+------------------+------------------+
// | discardable bytes |  readable bytes  |  writable bytes  |
// |                   |     (CONTENT)    |                  |
// +-------------------+------------------+------------------+
// |                   |                  |                  |
// 0      <=      readerIndex   <=   writerIndex    <=     size
// @endcode
class BytesBuffer
{
public:
	static const int kInitialSize = 1024;

	BytesBuffer()
		: _buffer(kInitialSize),
		_readerIndex(0),
		_writerIndex(0)
	{
		assert(readableBytes() == 0);
		assert(writableBytes() == kInitialSize);
		assert(discardableBytes() == 0);
	}

	// default copy-ctor, dtor and assignment are fine

	void swap(BytesBuffer& rhs)
	{
		_buffer.swap(rhs._buffer);
		std::swap(_readerIndex, rhs._readerIndex);
		std::swap(_writerIndex, rhs._writerIndex);
	}

	int readableBytes()
	{
		return _writerIndex - _readerIndex;
	}

	int writableBytes()
	{
		return _buffer.size() - _writerIndex;
	}

	int discardableBytes()
	{
		return _readerIndex;
	}

	const char* peek() const
	{
		return begin() + _readerIndex;
	}

	// retrieve returns void, to prevent
	// string str(retrieve(readableBytes()), readableBytes());
	// the evaluation of two functions are unspecified
	void retrieve(int length)
	{
		assert(length <= readableBytes());
		if (length < readableBytes())
		{
			_readerIndex += length;
		}
		else if (length == readableBytes())
		{
			retrieveAll();
		}
	}

	// length of buffer must >= length
	int retrieve(char* buffer, int length)
	{
        if (readableBytes() <= 0)
            return 0;
            
		if (length > readableBytes())
		{
			length = readableBytes();
			std::copy(peek(), peek() + length, buffer);
			retrieveAll();
		}
		else
		{
			std::copy(peek(), peek() + length, buffer);
			retrieve(length);
		}
		return length;
	}


	void retrieveAll()
	{
		_readerIndex = 0;
		_writerIndex = 0;
	}


	void retrieveUntil(const char* end)
	{
		assert(peek() <= end);
		assert(end <= beginWrite());
		retrieve(end - peek());
	}


	std::string retrieveAllAsString()
	{
		return retrieveAsString(readableBytes());;
	}

	std::string retrieveAsString(int length)
	{
		assert(length <= readableBytes());
		std::string result(peek(), length);
		retrieve(length);
		return result;
	}

	void append(const std::string& str)
	{
		append(str.data(), str.length());
	}

	void append(const char* data, int length)
	{
		if (writableBytes() < length)
		{
			makeSpace(length);
		}
		assert(length <= writableBytes());
		std::copy(data, data + length, beginWrite());
		_writerIndex += length;
	}


	void shrink(int reserve)
	{
		std::vector<char> buf(readableBytes() + reserve);
		std::copy(peek(), peek() + readableBytes(), buf.begin());
		buf.swap(_buffer);
	}

	char* begin()
	{
		return &*_buffer.begin();              
	}

	const char* begin() const
	{
		return &*_buffer.begin();
	}

private:
	char* beginWrite()
	{
		return begin() + _writerIndex;
	}

	const char* beginWrite() const
	{
		return begin() + _writerIndex;
	}

	void makeSpace(int more)
	{
		if (writableBytes() + discardableBytes() < more)
		{
			_buffer.resize(_writerIndex + more);
		}
		else
		{
			// move readable data to the front, make space inside buffer
			int used = readableBytes();
			std::copy(begin() + _readerIndex,
				begin() + _writerIndex,
				begin());
			_readerIndex = 0;
			_writerIndex = _readerIndex + used;
			assert(used == readableBytes());
		}
	}

private:
	std::vector<char> _buffer;
	int _readerIndex;
	int _writerIndex;
};



#endif  // ByteBuffer_Buffer_H
