#include "scripting.h"
#include "ir.h"

char	*fn_ir_emit(void **args)
{
	char	*protocol = ((char **)args)[0];
	int		address = *((int **)args)[1];
	int		command = *((int **)args)[2];
	e_ir_protocol	protocol_id = IR_NEC;

	if (!ft_strcmp(protocol, "NEC"))
		protocol_id = IR_NEC;
	else if (!ft_strcmp(protocol, "RC5"))
		protocol_id = IR_PHILIPS_RC5;
	else
	{
		log("IR.EMIT: Could not find a protocol called '%s'\n", LOG_ERROR | LOG_INDENT, protocol);
		return 0;
	}
	ir_transmit((uint8_t)(address & 0xFF), (uint8_t)(command & 0xFF), protocol_id);
	return 0;
}