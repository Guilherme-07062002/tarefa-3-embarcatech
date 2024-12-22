#include <stdio.h>
#include "pico/stdlib.h"

#define LED_VERMELHO 15
#define LED_AMARELO 14
#define LED_VERDE 13
#define LED_VERDE_PEDRESTES 12
#define BUZZER 21
#define BOTAO 10

// Enum para representar as cores do semáforo
typedef enum {
    VERDE,
    AMARELO,
    VERMELHO
} CorSemaforo;

// Função para acionar o sinal verde dos pedestres e buzzer com frequência ajustável
void sinal_verde_pedestre() {
    gpio_put(LED_VERDE_PEDRESTES, 1);  // Acende o LED verde para pedestres
    gpio_put(BUZZER, 1);  // Liga o buzzer
}

// Função para desligar o sinal verde dos pedestres e buzzer
void desligar_sinal_verde_pedestre() {
    gpio_put(LED_VERDE_PEDRESTES, 0);  // Desliga o LED verde para pedestres
    gpio_put(BUZZER, 0);               // Desliga o buzzer
}

// Função para alternar o semáforo de carros
void semaforo_carros() {
    // O funcionamento do semafóro dos carros será executado normalmente apenas 
    // quando o dos pedestres estiver desligado
    if (!gpio_get(LED_VERDE_PEDRESTES)) {
        ligarSemaforo(VERDE);
        // O verde fica ligado por 8 segundos
        verificarBotao(8);

        // O amarelo fica ligado por 2 segundos
        ligarSemaforo(AMARELO);
        verificarBotao(2);

        // E o vermelgo por 10 segundos
        ligarSemaforo(VERMELHO);
        verificarBotao(10);
    }
}

// Função para gerenciar o semáforo para pedestres
void semaforo_pedestres() {
    printf("Botão pressionado\n");

    // Desligar todos os semáforos
    desligarSemaforo(VERDE);
    desligarSemaforo(AMARELO);
    desligarSemaforo(VERMELHO);

    // Iniciar sequência de fechamento para os carros
    // O LED amarelo deve ficar acionado por 5 segundos
    ligarSemaforo(AMARELO);
    sleep_ms(5000);  

    // E depois o vermelho por 15 segundos
    ligarSemaforo(VERMELHO);
    sleep_ms(15000);  

    // Ligar semáforo dos pedestres
    sinal_verde_pedestre();

    sleep_ms(15000);  // Pedestre pode atravessar por 15 segundos

    // Em seguida, desligar semáforo dos pedestres e buzzer
    desligar_sinal_verde_pedestre();
    desligarSemaforo(VERMELHO);
}

// Função que espera por um determinado número de segundos ou interrompe se o botão for pressionado
void verificarBotao(int segundos) {
    for (int i = 0; i < segundos; i++) {
        sleep_ms(1000);  // Espera 1 segundo
        if (gpio_get(BOTAO) == 0) {  // Verifica se o botão foi pressionado
            return;  // Sai da função imediatamente
        }
    }
}

// Função para ligar o semáforo de acordo com a cor especificada
void ligarSemaforo(CorSemaforo cor) {
    // Desliga todos os LEDs antes de ligar o especificado
    desligarSemaforo(VERDE);
    desligarSemaforo(AMARELO);
    desligarSemaforo(VERMELHO);

    // Liga o LED correspondente
    switch (cor) {
        case VERDE:
            gpio_put(LED_VERDE, 1);
            break;
        case AMARELO:
            gpio_put(LED_AMARELO, 1);
            break;
        case VERMELHO:
            gpio_put(LED_VERMELHO, 1);
            break;
        default:
            printf("Cor inválida\n");
    }
}

// Função para desligar o semáforo de acordo com a cor especificada
void desligarSemaforo(CorSemaforo cor) {
    // Desliga o LED correspondente
    switch (cor) {
        case VERDE:
            gpio_put(LED_VERDE, 0);
            break;
        case AMARELO:
            gpio_put(LED_AMARELO, 0);
            break;
        case VERMELHO:
            gpio_put(LED_VERMELHO, 0);
            break;
        default:
            printf("Cor inválida\n");
    }
}

// Função para executar a inicialização dos pinos do circuito
void inicializarPinos() {
    gpio_init(LED_VERMELHO);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);

    gpio_init(LED_AMARELO);
    gpio_set_dir(LED_AMARELO, GPIO_OUT);

    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);

    gpio_init(LED_VERDE_PEDRESTES);
    gpio_set_dir(LED_VERDE_PEDRESTES, GPIO_OUT);

    gpio_init(BUZZER);
    gpio_set_dir(BUZZER, GPIO_OUT);

    gpio_init(BOTAO);
    gpio_set_dir(BOTAO, GPIO_IN);
    gpio_pull_up(BOTAO);

    stdio_init_all();
}

int main() {
    inicializarPinos();

    // Inicializa funcionamento padrão do semáforo
    while (1) {
        if (gpio_get(BOTAO) == 0) {
            sleep_ms(300);  // Debounce
            semaforo_pedestres();
        }

        semaforo_carros();
    }

    return 0;
}
