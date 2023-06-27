snapshots += Uart::App::Snapshot
snapshot_headers += apps/uart/app.h

app_objs += $(addprefix apps/uart/,\
  app.o\
  main_controller.o\
  monitor_controller.o\
)

i18n_files += $(addprefix apps/uart/,\
  base.universal.i18n\
)

ifeq ($(PLATFORM),device)
apps/uart/main_controller.o: SFLAGS += -DHAS_UART=1 -Iion/src/device
endif

app_images += apps/uart/uart_icon.png
