
main:
	@make -f m128.make test.a
	@cmd_ASA_loader -p 8 -h hex/test.hex
	@terminal -p 8
