#include "keyboard.h"
#include <io.h>
#include <stdbool.h>
#include <vga.h>
#include <interrupt.h>
#include <i8259a.h>

#define REG_DATA 0x60
#define REG_CMD 0x64
#define REG_STATUS 0x64

/* PS/2 controller commands */
#define CMD_CONFIG_READ 0x20
#define CMD_CONFIG_WRITE 0x60
/* if the second channel doesn't exist, these will simply be ignored */
#define CMD_PORT2_DISABLE 0xa7
#define CMD_PORT2_ENABLE 0xa8
#define CMD_PORT2_TEST 0xa9
#define CMD_SELF_TEST 0xaa
#define CMD_PORT1_TEST 0xab
#define CMD_PORT1_ENABLE 0xae
#define CMD_PORT1_DISABLE 0xad
#define CMD_WRITE_TO_PORT2 0xd4

#define DEVICE_CMD_RESET 0xff
#define DEVICE_RESPONSE_RESET_ACK 0xfa
#define DEVICE_RESPONSE_RESET_NACK 0xfc

#define PORT_SEND_NUM_RETRIES 1000

enum ps2_port {
    PORT1,
    PORT2
};

/* self test responses */
#define SELF_TEST_SUCCESS 0x55
#define SELF_TEST_FAILED 0xfc

enum {
    PS2_BUF_STATUS_CLEAR,
    PS2_BUF_STATUS_FULL
};

enum {
    DEVICE_DATA,
    CONTROLLER_CMD
};

enum {
    STATUS_REG_NOERROR,
    STATUS_REG_ERROR
};

enum {
    IRQ_STATUS_DISABLED,
    IRQ_STATUS_ENABLED
};

enum {
    SYSTEM_FLAG_POST_ERROR,
    SYSTEM_FLAG_POST_SUCCESS
};

enum {
    CLOCK_STATUS_ENABLED,
    CLOCK_STATUS_DISABLED
};

enum {
    TRANSLATION_DISABLED,
    TRANSLATION_ENABLED
};

struct ps2_status {
    uint8_t tx_buf_status : 1; /* must be set before attempting to read data reg */
    uint8_t rx_buf_status : 1; /* must be unset before attempting to write to data reg */
    uint8_t system_flag : 1;
    uint8_t cmd_or_data : 1;
    uint8_t unknown1 : 1;
    uint8_t unknown2 : 1;
    uint8_t timeout : 1;
    uint8_t parity : 1;
};
static_assert_sizeof(struct ps2_status, sizeof(uint8_t));

struct ps2_config {
    uint8_t port1_irq_status : 1;
    uint8_t port2_irq_status : 1;
    uint8_t system_flag : 1;
    uint8_t zero1 : 1;
    uint8_t port1_clock_status : 1;
    uint8_t port2_clock_status : 1;
    uint8_t port1_translation_status : 1;
    uint8_t zero2 : 1;
};
static_assert_sizeof(struct ps2_config, sizeof(uint8_t));

static bool is_dual_channel = false;
static bool should_probe_second_channel = true;

static void read_status(struct ps2_status *reg)
{
    *(uint8_t *)reg = inb(REG_STATUS);
}

static void read_config(struct ps2_config *config)
{
    outb(CMD_CONFIG_READ, REG_CMD);
    *(uint8_t *)config = inb(REG_DATA);
}

static void write_config(const struct ps2_config *config)
{
    outb(CMD_CONFIG_WRITE, REG_CMD);
    outb(*(uint8_t *)config, REG_DATA);
}

static bool i8042_self_test(void)
{
    uint8_t response;
    outb(CMD_SELF_TEST, REG_CMD);
    response = inb(REG_DATA);

    while (response != SELF_TEST_SUCCESS && response != SELF_TEST_FAILED)
        response = inb(REG_DATA);

    return response == SELF_TEST_SUCCESS;
}

static void disable_devices(void)
{
    struct ps2_status status_reg;
    struct ps2_config config;
    outb(CMD_PORT1_DISABLE, REG_CMD);
    outb(CMD_PORT2_DISABLE, REG_CMD);
    read_status(&status_reg);
    
    /* flush output buffer */
    while (status_reg.tx_buf_status != PS2_BUF_STATUS_CLEAR) {
        native_io_delay();
        (void)inb(REG_DATA);
        read_status(&status_reg);
    }

    read_config(&config);
    config.port1_irq_status = IRQ_STATUS_DISABLED;
    config.port2_irq_status = IRQ_STATUS_DISABLED;
    config.port1_translation_status = TRANSLATION_DISABLED;

    if (config.port2_clock_status == CLOCK_STATUS_ENABLED) {
        /*
         * we detected the clock is enabled even though we've disabled the 2nd port. this weird behavior means that
         * there's definitely no second channel.
         */
        is_dual_channel = false;
        should_probe_second_channel = false;
        vga_print_string("PS/2 2nd channel disabled.");
    }

    write_config(&config);
}

static bool is_channel2_present(void)
{
    struct ps2_config config;
    outb(CMD_PORT2_ENABLE, REG_CMD);
    read_config(&config);
    bool ret = config.port2_clock_status == CLOCK_STATUS_ENABLED;
    outb(CMD_PORT2_DISABLE, REG_CMD);
    return ret;
}

static bool test_ports(void)
{
    outb(CMD_PORT1_TEST, REG_CMD);
    uint8_t response = inb(REG_DATA);
    
    if (response != 0)
        return false;

    if (is_dual_channel) {
        outb(CMD_PORT2_TEST, REG_CMD);
        response = inb(REG_DATA);

        if (response != 0)
            return false;
    }
    
    return true;
}

static void enable_ports(void)
{
    struct ps2_config config;
    outb(CMD_PORT1_ENABLE, REG_CMD);
    outb(CMD_PORT2_ENABLE, REG_CMD);
    read_config(&config);
    config.port1_irq_status = IRQ_STATUS_ENABLED;
    config.port2_irq_status = IRQ_STATUS_ENABLED;
    write_config(&config);
}

static bool send_port(enum ps2_port port, uint8_t data)
{
    struct ps2_status status;
    read_status(&status);
    int retries = PORT_SEND_NUM_RETRIES;

    if (port == PORT2) {
        if (!is_dual_channel)
            return false;
        
        outb(CMD_WRITE_TO_PORT2, REG_CMD);
    }

    while (status.rx_buf_status != PS2_BUF_STATUS_CLEAR && retries > 0) {
        native_io_delay();
        read_status(&status);
        --retries;
    }

    if (retries == 0)
        return false;

    outb(data, REG_DATA);
    return true;
}

static bool reset_devices(void)
{
    bool ret = send_port(PORT1, DEVICE_CMD_RESET);
    
    if (!ret)
        return ret;

    if (is_dual_channel)
        ret = send_port(PORT2, DEVICE_CMD_RESET);
    
    return ret;
}

__interrupt void keyboard_interrupt_handler(struct interrupt_frame *frame)
{
    (void)inb(0x60);
	vga_print_string("key.\n");
	i8259A_ack_irq(i8259A_MASTER);
}

bool keyboard_init(void)
{
    /* assume PS/2 controller is present. */
    disable_devices();
    
    if (!i8042_self_test()) {
        vga_print_string("PS/2 controller self test failed!!!\n");
        return false;
    
    }

    if (should_probe_second_channel) {
        /* make sure the second channel is present. */
        is_dual_channel = is_channel2_present();
    }

    if (!test_ports()) {
        vga_print_string("PS2 ports test failed.\n");
        return false;
    }

    enable_ports();
    interrupt_add(ISA_INTERUPTS_VECTOR + 1, keyboard_interrupt_handler);
    
    if (!reset_devices()) {
        vga_print_string("Failed to send reset command to PS2 devices\n");
        return false;
    }

    return true;
}