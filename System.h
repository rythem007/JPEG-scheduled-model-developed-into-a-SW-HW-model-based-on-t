//-----Definition of the hierarchical system module---------------
//-----You must modify this file as indicated by TODO comments----

#include "systemc.h"
#include "Block.h"
#include "CPU.h"
#include "HW_Quant.h"

//top-level module
SC_MODULE(System)
{
private:
	// define the SW-HW modules
	CPU *cpu;
	HW_Quant *hw_quant;

	// instantiation of the top level FIFOs
	sc_fifo<Block> FIFO_DCT2Quant, FIFO_Quant2Zigzag;

	// instantiation of the interrupts
	sc_signal<bool> interrupt_Quant_input_seek, interrupt_Quant_output_avail;

public:
	//constructor
	SC_CTOR(System):FIFO_DCT2Quant(FIFO_SIZE), 
			FIFO_Quant2Zigzag(FIFO_SIZE) {

		// TODO: instantiate the modules
		
		cpu = new CPU("CPU");
		hw_quant = new HW_Quant("HW_Quant");

		// TODO: bind the FIFO ports
		
		cpu->DCT2Quant.bind(FIFO_DCT2Quant);
		cpu->Quant2Zigzag.bind(FIFO_Quant2Zigzag);
		
	    hw_quant->InBuffer.bind(FIFO_DCT2Quant);
	    hw_quant->OutBuffer.bind(FIFO_Quant2Zigzag);
		
		// TODO:bind the interrupt ports
		
		cpu->int_Quantize_input_seek.bind(interrupt_Quant_input_seek);
		cpu->int_Quantize_output_avail.bind(interrupt_Quant_output_avail);
		
	    hw_quant->int_input_seek.bind(interrupt_Quant_input_seek);
	    hw_quant->int_output_avail.bind(interrupt_Quant_output_avail);
		
	}
};

