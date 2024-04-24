CC=gcc
CFLAGS=-Wall -g
HEADERS=test/minunit.h lib/buttons.h lib/device.h lib/display.h lib/lcd.h lib/log.h lib/colors.h lib/fonts/fonts.h bitcoin/ecc/s256field.h bitcoin/ecc/s256point.h bitcoin/ecc/signature.h bitcoin/helper/helper.h bitcoin/ecc/privatekey.h bitcoin/tx/tx.h bitcoin/tx/txin.h bitcoin/tx/txout.h bitcoin/tx/txfetcher.h
SRCS=main.c test/minunit.c lib/buttons.c lib/device.c lib/display.c lib/lcd.c lib/log.c lib/fonts/font8.c lib/fonts/font12.c lib/fonts/font16.c lib/fonts/font20.c lib/fonts/font24.c bitcoin/ecc/s256field.c bitcoin/ecc/s256point.c bitcoin/ecc/signature.c bitcoin/helper/helper.c bitcoin/ecc/privatekey.c bitcoin/tx/tx.c bitcoin/tx/txin.c bitcoin/tx/txout.c bitcoin/tx/txfetcher.c
TEST_SRCS=test/minunit.c lib/buttons.c lib/device.c lib/display.c lib/lcd.c lib/log.c lib/fonts/font8.c lib/fonts/font12.c lib/fonts/font16.c lib/fonts/font20.c lib/fonts/font24.c bitcoin/ecc/s256field.c bitcoin/ecc/s256point.c bitcoin/ecc/signature.c bitcoin/helper/helper.c bitcoin/ecc/privatekey.c bitcoin/tx/tx.c bitcoin/tx/txin.c bitcoin/tx/txout.c bitcoin/tx/txfetcher.c
OBJS=$(SRCS:.c=.o)
TEST_OBJS=$(TEST_SRCS:.c=.o)
TARGET=main
NAME=BitcoinSignerExecutable
TEST_NAME=TestExecutable

# Rule for the main target
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(filter-out test/minunit.o, $(OBJS)) -o $(NAME) -lbcm2835 -lgmp -lcrypto -lcurl -ljansson

# Rule for object files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule for cleaning up
clean:
	rm -f $(TARGET) $(OBJS) $(NAME) $(TEST_NAME) $(TEST_OBJS)

# Rule for tests
test: $(TEST_OBJS)
	$(CC) $(CFLAGS) $(TEST_OBJS) -o $(TEST_NAME) -lbcm2835 -lgmp -lcrypto -lcurl -ljansson