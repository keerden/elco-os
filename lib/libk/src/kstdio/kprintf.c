#include <kstdbool.h>
#include <kstdarg.h>
#include <kstdio.h>
#include <kstring.h>


static void print(const char* data, size_t data_length);
static char* i_convert (char* s, unsigned long long val, int base, int digits);
static size_t i_print(unsigned long long val, int base);






int kprintf(const char* restrict format, ...)
{
	va_list parameters;
	va_start(parameters, format);

	size_t written = 0;
	size_t amount, len;

	char c;
	const char* s;
	int i;
	unsigned int u;

	while ( *format != '\0' )
	{
		if ( *format == '%' )
		{
			switch(*(++format))
			{
				case 'c':
					c = (char) va_arg(parameters, int /* char promotes to int */);
					print(&c, sizeof(c));
					written++;
					break;
				case 's':
					s = va_arg(parameters, const char*);
					len = kstrlen(s);
					print(s, len);
					written += len;					
					break;
				case 'd':
				case 'i':
					i = va_arg(parameters, int);
					if(i < 0)
					{
						print("-", 1);
						written++;
						i = -i;
					}
					written += i_print((unsigned long long) i, 10);
					break;
				case 'u':
					u = va_arg(parameters, unsigned int);
					written += i_print(u, 10);
					break;				
				case 'x':
				case 'p':
					u = va_arg(parameters, unsigned int);
					written += i_print(u, 16);
					break;
				case 'o':
					u = va_arg(parameters, unsigned int);
					written += i_print(u, 8);
					break;
				case '\0':
					format--;
				break;	
				default:	
					if (*format != '%')
					{
						print("%", 1);	
						written++;
					}	
					print(format, 1);
					written++;
					break;
			}
			format++;
		}else {
			amount = 0;
			while ( format[amount] && format[amount] != '%' )
				amount++;
			print(format, amount);
			format += amount;
			written += amount;
		}
	}

	va_end(parameters);

	return (int) written;
}





static void print(const char* data, size_t data_length)
{
	for ( size_t i = 0; i < data_length; i++ )
		kputchar((int) ((const unsigned char*) data)[i]);
}



static char* i_convert (char* s, unsigned long long val, int base, int digits)
{
	int d;
	
	d = (int) (val % ( unsigned long long) base);
	val /= (unsigned long long) base;
	if(val || digits > 1)
		s = i_convert (s, val, base, digits - 1);	
	*(s++) = (char) (d>9 ? ('a' + d - 10) : ('0' + d));	

	return s;
}

static size_t i_print(unsigned long long val, int base)
{
	char buff[65];
	char* s = buff;
	size_t size;


	s = i_convert(s, val, base, 1);
	size = (size_t) (s - buff);
	print(buff, size);
	return size;
	
}