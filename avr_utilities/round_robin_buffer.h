#ifndef ROUND_ROBIN_BUFFER_H
#define ROUND_ROBIN_BUFFER_H

#include <stdint.h>

template<uint8_t buffer_size = 64, typename datatype = uint8_t>
struct round_robin_buffer
{
public:

    typedef datatype value_type;


    /// tentative writes, write to the buffer, but don't
    /// make the data available to readers yet.
    bool __attribute__((noinline)) write_tentative( value_type value) volatile
    {
        if (!is_full)
        {
            buffer[tentative_index] = value;
            tentative_index = (tentative_index + 1) % buffer_size;
            is_full = tentative_index == read_index;
            return true;
        }
        else
        {
            return false;
        }
    }

    /// remove all tentative writes
    void reset_tentative() volatile
    {
        if (tentative_index != write_index)
        {
            is_full = false;
        }
        tentative_index = write_index;
    }


    /// commit all tentative writes, making them
    /// available to readers.
    void commit() volatile
    {
        write_index = tentative_index;
    }

/*
    /// write a value to the queue. 
    /// Don't mix write calls with write_tentative.
    bool write( value_type value) volatile
    {

        if (!is_full)
        {
            buffer[write_index] = value;
            write_index = (write_index + 1) % buffer_size;
            if (write_index == read_index)
            {
                is_full = true;
            }
            return true;
        }
        else
        {
            return false;
        }
    }
*/
    /// read a value from the queue
    bool read(  value_type *value) volatile
    {
        if (get_first( value))
        {
            read_index = (read_index + 1) % buffer_size;
 			is_full = false;
 
            return true;
        }
        else
        {
            return false;
        }
    }

    bool get_first( value_type *value) const volatile
    {
        if (read_index == write_index && !is_full)
        {
            return false;
        }
        else
        {
            *value = buffer[read_index];
            return true;
        }

    }

    /// wait for a value and read it.
    value_type read_w() volatile
    {
        value_type value;
        while (!read(&value)) /*nop*/ ;
        return value;
    }

    // wait for the buffer to become not full and write
    void write_tentative_w( value_type value) volatile
	{
    	while (!write_tentative( value)) /*nop*/;
	}

    /// return the number of (committed) bytes in the buffer
    uint8_t size() const volatile
	{
		if (is_full) return buffer_size;

		const int8_t size = (int8_t) write_index - (int8_t)read_index;
		if (size < 0)
		{
			return buffer_size + size;
		}
		else
		{
			return size;
		}
	}

    /// return whether the buffer is empty
    bool empty() const volatile
	{
    	return write_index == read_index && !is_full;
	}

    bool full() const volatile
	{
    	return is_full;
	}
private:
    bool    is_full;
    uint8_t tentative_index;
    uint8_t write_index;
    uint8_t read_index;
    value_type buffer[buffer_size];

};


#endif //ROUND_ROBIN_BUFFER_H
