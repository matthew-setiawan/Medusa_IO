#include "system.h"
#include "altera_up_avalon_accelerometer_spi.h"
#include "altera_avalon_timer_regs.h"
#include "altera_avalon_timer.h"
#include "altera_avalon_pio_regs.h"
#include "sys/alt_stdio.h"
#include "sys/alt_irq.h"
#include "sys/alt_timestamp.h"


#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#define OFFSET -32
#define PWM_PERIOD 16

//UART input parsing
#define TOKEN_SIZE 8
#define BUFFER_SIZE 64

//UART output parsing
#define HEX_BUF_SIZE 8

//UART input buffer
char* cmdbuffer;

//Hex buffer declaration
char* hexbuffers[3];

//Hex display buffer
#define DISP_BUF_SIZE 10
char* disp_buf;

//Global accelerometer declaration
alt_up_accelerometer_spi_dev * acc_dev;

//Buffer typing & coeffs
#define RING_T alt_32
#define RING_SIZE 29
double h[] = {0.0030, 0.0114, -0.0179, -0.0011, 0.0223, -0.0225, -0.0109, 0.0396, -0.0263,\
		-0.0338, 0.0752, -0.0289, -0.1204, 0.2879, 0.6369, 0.2879, -0.1204, -0.0289, 0.0752, \
		-0.0338, -0.0263, 0.0396, -0.0109, -0.0225, 0.0223, -0.0011, -0.0179, 0.0114, 0.0030};

//X,Y,Z accelerometer buffers
struct ring_buffer *x_buf;
struct ring_buffer *y_buf;
struct ring_buffer *z_buf;

//Forward declarations

void timer_init(void * isr);
void throw_code(char* regname, int code);
alt_32 get_input(char x);
void read_request();
void to_hex(alt_32 val, int length, char* buf );
void parse_request(char* request);
//int write_to_disp(char* str, int offset);


//Ring buffer
//---------------------------------------------------------------
struct ring_buffer {
	int size;
	int next_free;
	RING_T* values;
};

//Buffer functions
void ring_buf_push(struct ring_buffer* buf, RING_T in){
	if (buf->next_free < 0){
		buf->next_free = buf ->size-1;
	}
	(buf->values)[buf->next_free] = in;
	(buf->next_free)--;
}

RING_T ring_buf_read(struct ring_buffer* buf, RING_T idx){
	int mapped_idx;
	if (buf->next_free+1+idx >= buf->size){
		mapped_idx = buf->next_free+1+idx - buf->size;
	}
	else {
		mapped_idx = buf->next_free+1+idx;
	}

	return buf->values[mapped_idx];
}

alt_32 convolve_float(struct ring_buffer* buf, double coefficients[]  ){

	//Disabling interrupts prevents accelerometer buffers being overwritten while value is calculated
	//alt_irq_context state = alt_irq_disable_all ();
	alt_irq_disable(TIMER_IRQ);

	double sum = 0;
	for(int i = 0; i < buf->size; i ++){
		sum += ring_buf_read(buf, i)* coefficients[i];
	}

	//Re-enable interrupts from state
	//alt_irq_enable_all(state);
	alt_irq_enable(TIMER_IRQ);
	return (alt_32)sum;

}
// Timer code
//---------------------------------------------------------------



void timer_init(void * isr) {
	//Calculate necessary cycles for 1 ms period
	alt_32 period = alt_timestamp_freq()/1000;


    IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_BASE, 0x0003);
    IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_BASE, 0);
    IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER_BASE, (alt_16) period );
    IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER_BASE, 0xfbd0);
    IOWR_ALTERA_AVALON_TIMER_PERIODH(TIMER_BASE, 0x0001);
    alt_irq_register(TIMER_IRQ, 0, isr);
    IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_BASE, 0x0007);

}

void sys_timer_isr() {
    IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_BASE, 0);


    alt_32 x,y,z;

    alt_up_accelerometer_spi_read_x_axis(acc_dev, & x);
	alt_up_accelerometer_spi_read_y_axis(acc_dev, & y);
	alt_up_accelerometer_spi_read_z_axis(acc_dev, & z);

	ring_buf_push(x_buf, x);
	ring_buf_push(y_buf, y);
	ring_buf_push(z_buf, z);

	//iNTERRUPT FREQUENCY MONITORING
	static int count;
	static alt_64 lasttime;

    if(count%1024 == 0){
    	//printf("%i\n", (alt_timestamp()-lasttime)/1024);
    	count = 0;
    	lasttime = alt_timestamp();
    }
    count++;

	//alt_putstr("got here \n");

}


//Input tokenization and parsing
//---------------------------------------------------------------
void read_request(char* outbuf){
	char c;
	int idx = 0;

	//Clear previous request from buffer
	memset(outbuf, 0, BUFFER_SIZE);

	//Get first character
	c = alt_getchar();

	while(c != '\n'){
		outbuf[idx] = c;
		c = alt_getchar();
		idx++;
	}
	outbuf[idx] = ' ';

}

int write_to_disp(char* str, int offset){
	//HEX0 rightmost digit

	 //char_to_hex(str[0]);
	IOWR_ALTERA_AVALON_PIO_DATA(HEX0_BASE, 127);


//	IOWR_ALTERA_AVALON_PIO_DATA(HEX5_BASE, let5);
//	IOWR_ALTERA_AVALON_PIO_DATA(HEX4_BASE, let4);
//	IOWR_ALTERA_AVALON_PIO_DATA(HEX3_BASE, let3);
//	IOWR_ALTERA_AVALON_PIO_DATA(HEX2_BASE, let2);
//	IOWR_ALTERA_AVALON_PIO_DATA(HEX1_BASE, let1);
//	IOWR_ALTERA_AVALON_PIO_DATA(HEX0_BASE, let0);
	return offset;
}

void parse_request(char* request){
	int token_number = 0;
	char* tokens[8] = {};
	char* token;


	token = strtok(cmdbuffer, " ");


	while(token != NULL){
		tokens[token_number]= token;
		token_number++;
		token = strtok(NULL, " ");
	}

	if(token_number == 0 || strcmp(tokens[0], &"R" ) != 0){
		throw_code(&"ERR", 1);
		return;
	}

	//Read request parsing
	int is_all = (strcmp(tokens[1], &"ALL") == 0); //Strcmp is weird and equal is denoted by 0
	int matched = 0;

	if(is_all || (strcmp(tokens[1], &"ACCPROC") == 0)){
		//alt_printf("Tried to read accproc");
		matched = 1;

		alt_32 x,y,z;
			x = convolve_float(x_buf, h);
			y = convolve_float(y_buf, h);
			z = convolve_float(z_buf, h);

			to_hex(x, 3, hexbuffers[0]);
			to_hex(y, 3, hexbuffers[1]);
			to_hex(z, 3, hexbuffers[2]);
			printf("K ACCPROC X%sY%sZ%s 0\n", hexbuffers[0], hexbuffers[1], hexbuffers[2]);

	}
	if (is_all || strcmp(tokens[1], &"ACCRAW") == 0){
		matched = 1;

		//Direct accelerometer read
		alt_32 x,y,z;
		alt_up_accelerometer_spi_read_x_axis(acc_dev, & x);
		alt_up_accelerometer_spi_read_y_axis(acc_dev, & y);
		alt_up_accelerometer_spi_read_z_axis(acc_dev, & z);

		to_hex(x, 3, hexbuffers[0]);
		to_hex(y, 3, hexbuffers[1]);
		to_hex(z, 3, hexbuffers[2]);
		printf("K ACCRAW X%sY%sZ%s 0\n", hexbuffers[0], hexbuffers[1], hexbuffers[2]);

	}
	if (is_all || strcmp(tokens[1], &"BUTTON") == 0){

		matched = 1;
		alt_32 button = ~IORD_ALTERA_AVALON_PIO_DATA(BUTTON_BASE);

		//Button response processing
		//Bitmask
		alt_32 mask = 0x3;
		button = button & mask;
		alt_printf("K BUTTON %x 0\n", button);
	}
	if (is_all || strcmp(tokens[1], &"SWITCH") == 0){
		//alt_printf("Tried to read switch");
		matched = 1;

		alt_32 switches = ~IORD_ALTERA_AVALON_PIO_DATA(SWITCH_BASE);
		switches &= 0x3ff;
		to_hex(switches, 3, hexbuffers[0]);
		alt_printf("K SWITCH %x 0", hexbuffers[0]);

	}


	//Write request parsing

	if (strcmp(tokens[1], &"HEXTEXT") == 0){
		//alt_printf("Tried to write HEXTEXT");
		matched = 1;
		//Copy first 6 characters
		strncpy(disp_buf, tokens[2], DISP_BUF_SIZE);
		int k = write_to_disp(&" ", 0);
		throw_code("HEXTEXT", k);
	}

	if (strcmp(tokens[1], &"LEDFLASH") == 0){
		//alt_printf("Tried to write LEDFLASH");
		matched = 1;

		int k = atoi(tokens[2]);
		IOWR(LED_BASE, 0, k);
		throw_code(&"LEDFLASH", 0);
		return;
	}

	if (!matched){
		throw_code(&"ERR", 2);
		return;
	}

}

//Input processing
//---------------------------------------------------------------
//Function takes integer and writes to character buffer the hex representation in
//such a way that it is padded or cut down to length

void to_hex(alt_32 val, int length, char* buf ){
	//Clear previous data from buffer and write new
	memset(buf, 0, HEX_BUF_SIZE);
	sprintf(buf, "%x", val);
	int hexlen = strlen(buf);

	//Left shift if string is longer than desired length to take least significant
	if (hexlen > length){
		for(int i = 0; i < length; i++){
			buf[i] = buf[(i + hexlen - length)];
		}
	}
	//Right shift to align if desired hex is shorter than desired
	else if (hexlen < length){
		for(int i = 0; i < length; i++){
			if(i < hexlen){
				buf[(length - 1 - i)] = buf[(length - 1 - i) - (length-hexlen)];
			}
			else{
				buf[(length - 1 - i)] = '0';
			}

		}
	}
	//Add termination
	buf[length] = '\0';

}


alt_32 get_input(char x){

	alt_32 thresh = 150;

	//Temp gating code
	if(x == 'x'){
		alt_32 x = convolve_float(x_buf, h);
		if(x<-thresh)x = 15;
		else if(x>thresh) x = 1;
		else x = 0;

		return x;

	}
	else if(x == 'y'){
		alt_32 y = alt_up_accelerometer_spi_read_y_axis(acc_dev, & y);
		if(y<-thresh)y = 15;
		else if(y>thresh) y = 1;
		else y = 0;

		return y;

	}
	else if(x == 'z'){
		alt_32 z = alt_up_accelerometer_spi_read_z_axis(acc_dev, & z);
		if(z<-thresh)z = 15;
		else if(z>thresh) z = 1;
		else z = 0;

		return z;

	}
	else{
		//throw_code(3);
		return 16;
	}
}


void throw_code(char* regname, int code){
	printf("K %s %x\n", regname, code);
}

//HEX write
//---------------------------------------------------------------


int main() {


	//UART buffer instantiation
	cmdbuffer = malloc( BUFFER_SIZE * sizeof(char));

	//Initialize hex output buffers
	hexbuffers[0] = malloc( HEX_BUF_SIZE * sizeof(char));
	hexbuffers[1] = malloc( HEX_BUF_SIZE * sizeof(char));
	hexbuffers[2] = malloc( HEX_BUF_SIZE * sizeof(char));

	//Initialize accelerometer buffers
	struct ring_buffer x,y,z;

	x = (struct ring_buffer){.size = RING_SIZE, .next_free = 0, .values = 0};
	y = (struct ring_buffer){.size = RING_SIZE, .next_free = 0, .values = 0};
	z = (struct ring_buffer){.size = RING_SIZE, .next_free = 0, .values = 0};
	x_buf = &x;
	y_buf = &y;
	z_buf = &z;

	x_buf->values = malloc((RING_SIZE) * sizeof(RING_T));
	memset(x_buf->values, 0,(RING_SIZE) * sizeof(RING_T));
	y_buf->values = malloc((RING_SIZE) * sizeof(RING_T));
	memset(y_buf->values, 0,(RING_SIZE) * sizeof(RING_T));
	z_buf->values = malloc((RING_SIZE) * sizeof(RING_T));
	memset(x_buf->values, 0,(RING_SIZE) * sizeof(RING_T));

	//Display buffer
	disp_buf  = malloc((DISP_BUF_SIZE) * sizeof(char));
	memset(disp_buf, 0,(DISP_BUF_SIZE) * sizeof(char));

	//Accelerometer initialization
	acc_dev = alt_up_accelerometer_spi_open_dev("/dev/accelerometer_spi");


	//to_hex(16,3,hexbuffers[0]);


	//1kHz routine initialization
	timer_init(sys_timer_isr);
	alt_timestamp_start();

	//Command response loop
	while(1){
	read_request(cmdbuffer);
	parse_request(cmdbuffer);
	}

    return 0;
}
