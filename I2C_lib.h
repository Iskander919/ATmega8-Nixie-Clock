void I2C_start()
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTA);
	while (!(TWCR & (1<<TWINT)));
}

void I2C_stop()
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
}

void I2C_init()
{
	TWBR = 2; // Настройка частоты шины
	TWSR = (1 << TWPS1)|(1 << TWPS0); // Предделитель на 64
	TWCR |= (1 << TWEN); // Включение модуля TWI
}

void I2C_send_data(char data)
{
	TWDR = data;
	TWCR = (1<<TWINT)|(1<<TWEN);
	while(!(TWCR& (1<<TWINT)));
	
}

char I2C_read_data()
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
	while(!(TWCR& (1<<TWINT)));
	return TWDR;
}

char I2C_read_data_kek()
{
	TWCR = (1<<TWINT)|(1<<TWEN);
	while(!(TWCR& (1<<TWINT)));
	return TWDR;
}