#include "setup.h"

/**
 * Atualiza os valores globais do sistema.
 * 
 * Esta função lê o joystick, mede a distância com o sensor ultrassônico
 * e atualiza os estados dos botões, armazenando os resultados em variáveis
 * globais compartilhadas.
 */

void updateValues() {
    readJoystick();
    measureDistance();
    updateButtonStates();
}

/**
 * Função principal do programa.
 *
 * Inicializa o sistema, conecta ao Wi-Fi e estabelece uma conexão TCP.
 * Em um loop infinito, atualiza os valores dos sensores e botões,
 * envia os dados ao servidor e aguarda um intervalo de 1 segundo.
 */

/**
 * Fun o principal do programa.
 *
 * Inicializa o sistema, conecta ao Wi-Fi e estabelece uma conex o TCP.
 * Em um loop infinito, atualiza os valores dos sensores e bot es,
 * envia os dados ao servidor e aguarda um intervalo de 1 segundo.
 */
int main() {
    initializeSystem();

    // Aguarda 200 ms antes de conectar ao Wi-Fi
    sleep_ms(200);
    connectWifi();

    // Estabelece uma conex o TCP com o servidor
    sleep_ms(1000);  // Aguarda 1 segundos antes de tentar conexão
    create_tcp_connection();

    // Loop infinito que atualiza os valores dos sensores e botões e
    // envia os dados ao servidor
    while (true) {
        updateValues(); // Atualiza os valores dos sensores e botões
        send_data_to_server(); // Envia os dados ao servidor

        sleep_ms(1000);
    }

    // Nunca alcan ada, pois o loop   infinito
    return 0;
}
