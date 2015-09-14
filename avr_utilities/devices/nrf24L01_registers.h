//
//  Copyright (C) 2015 Danny Havenith
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#include "avr_utilities/datasheet_registers.hpp"

/* Memory Map */
namespace {

AVRUTIL_DATASHEET_REGISTER(
    CONFIG, 0x0,
        (mask_rx_dr,    6)
        (mask_tx_ds,    5)
        (mask_max_rt,   4)
        (en_crc,        3)
        (crco,          2)
        (pwr_up,        1)
        (prim_rx,       0)
)

AVRUTIL_DATASHEET_REGISTER(
    EN_AA, 0x01,
        (enaa_p5, 5)
        (enaa_p4, 4)
        (enaa_p3, 3)
        (enaa_p2, 2)
        (enaa_p1, 1)
        (enaa_p0, 0)
)

AVRUTIL_DATASHEET_REGISTER(
    EN_RXADDR, 0x02,
        (erx_p5, 5)
        (erx_p4, 4)
        (erx_p3, 3)
        (erx_p2, 2)
        (erx_p1, 1)
        (erx_p0, 0)
)

AVRUTIL_DATASHEET_REGISTER(
    SETUP_AW, 0x03,
        (aw, 1, 0)
)

AVRUTIL_DATASHEET_REGISTER(
    SETUP_RETR, 0x04,
        (ard, 7, 4)
        (arc, 3, 0)
)

AVRUTIL_DATASHEET_REGISTER(
    RF_CH, 0x05,
        (rf_ch, 6, 0)
)

AVRUTIL_DATASHEET_REGISTER(
    RF_SETUP, 0x06,
        (cont_wave, 7)
        (rf_dr_low, 5)
        (pll_lock,  4)
        (rf_dr_high,3)
        (rf_pwr,    1)
)

AVRUTIL_DATASHEET_REGISTER(
    STATUS, 0x07,
        (rx_dr,     6)
        (tx_ds,     5)
        (max_rt,    4)
        (rx_p_no,   3,1)
        (tx_full,   0)
)

AVRUTIL_DATASHEET_REGISTER(
    OBSERVE_TX, 0x08,
        (plos_cnt, 7,4)
        (arc_cnt,  3,0)
)

AVRUTIL_DATASHEET_REGISTER(
    RPD, 0x09,
        (rpd, 0)
)

custom_register_type< 0x0a,
    uint8_t [5]> rx_addr_p0;

custom_register_type< 0x0b,
    uint8_t [5]> rx_addr_p1;

AVRUTIL_DATASHEET_REGISTER(
    RX_ADDR_P2, 0x0c,
        (rx_addr_p2, 7,0)
)

AVRUTIL_DATASHEET_REGISTER(
    RX_ADDR_P3, 0x0d,
        (rx_addr_p3, 7,0)
)

AVRUTIL_DATASHEET_REGISTER(
    RX_ADDR_P4, 0x0e,
        (rx_addr_p4, 7,0)
)

AVRUTIL_DATASHEET_REGISTER(
    RX_ADDR_P5, 0x0f,
        (rx_addr_p5, 7,0)
)

custom_register_type< 0x10,
    uint8_t [5]> tx_addr;


AVRUTIL_DATASHEET_REGISTER(
    RX_PW_P0, 0x11,
        (rx_pw_p0, 5,0)
)

AVRUTIL_DATASHEET_REGISTER(
    RX_PW_P1, 0x12,
        (rx_pw_p1, 5,0)
)

AVRUTIL_DATASHEET_REGISTER(
    RX_PW_P2, 0x13,
        (rx_pw_p2, 5,0)
)

AVRUTIL_DATASHEET_REGISTER(
    RX_PW_P3, 0x14,
        (rx_pw_p3, 5,0)
)

AVRUTIL_DATASHEET_REGISTER(
    RX_PW_P4, 0x15,
        (rx_pw_p4, 5,0)
)

AVRUTIL_DATASHEET_REGISTER(
    RX_PW_P5, 0x16,
        (rx_pw_p5, 5,0)
)

AVRUTIL_DATASHEET_REGISTER(
    FIFO_STATUS, 0x17,
        (tx_reuse,  6)
        (fifo_full, 5)
        (tx_empty,  4)
        (rx_full,   1)
        (rx_empty,  0)
)

AVRUTIL_DATASHEET_REGISTER(
    DYNPD, 0x1c,
        (dpl_p5, 5)
        (dpl_p4, 4)
        (dpl_p3, 3)
        (dpl_p2, 2)
        (dpl_p1, 1)
        (dpl_p0, 0)
)

AVRUTIL_DATASHEET_REGISTER(
    FEATURE, 0x1d,
        (en_dpl,     2)
        (en_ack_pay, 1)
        (en_dyn_ack, 0)
)

/* Instruction Mnemonics */
const uint8_t R_REGISTER    = 0x00;
const uint8_t W_REGISTER    = 0x20;
const uint8_t REGISTER_MASK = 0x1F;
const uint8_t R_RX_PAYLOAD  = 0x61;
const uint8_t R_RX_PL_WIDTH = 0x60;
const uint8_t W_TX_PAYLOAD  = 0xA0;
const uint8_t FLUSH_TX      = 0xE1;
const uint8_t FLUSH_RX      = 0xE2;
const uint8_t REUSE_TX_PL   = 0xE3;
const uint8_t NOP           = 0xFF;
}
