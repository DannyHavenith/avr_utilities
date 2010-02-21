#include "round_robin_buffer.h"

bool round_robin_buffer::write( uint8_t value) volatile
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

bool round_robin_buffer::write_tentative( uint8_t value) volatile
{
    if (
            tentative_index != read_index 
        ||  (tentative_index == write_index && !is_full)
        )
    {
        buffer[tentative_index] = value;
        tentative_index = (tentative_index + 1) % buffer_size;

        return true;
    }
    else
    {
        return false;
    }
}

void round_robin_buffer::reset_tentative() volatile
{
    tentative_index = write_index;
}

void round_robin_buffer::commit_tentative() volatile
{
    write_index = tentative_index;
    is_full = (write_index == read_index);
}

bool round_robin_buffer::read( uint8_t *value) volatile
{
    if (read_index == write_index && !is_full)
    {
        return false;
    }
    else
    {
        *value = buffer[read_index];
        read_index = (read_index + 1) % buffer_size;
        is_full = false;
        return true;
    }
}

uint8_t round_robin_buffer::read_w( ) volatile
{
    uint8_t value;
    while (!read(&value)) /*nop*/ ;
    return value;
}
