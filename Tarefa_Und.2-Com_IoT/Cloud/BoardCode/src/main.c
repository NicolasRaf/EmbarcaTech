#include "setup.h"

void updateValues() {
    read_joystick();
    measureDistance();
    update_button_states();
}

int main() {
    initializeSystem();

    sleep_ms(200);
    connectWifi();
    create_tcp_connection();

    while (true) {
        updateValues();
        send_data_to_server();
        sleep_ms(1000);
    }

    return 0;
}