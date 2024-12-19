#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define LED_VERMELHO 15
#define LED_AMARELO 14
#define LED_VERDE 13
#define LED_VERDE_PEDRESTES 12
#define BUZZER 11
#define BOTAO 10

// Função para acionar o sinal verde dos pedestres e buzzer
void sinal_verde_pedestre() {
    gpio_put(LED_VERDE_PEDRESTES, 1);  // Acende o LED verde para pedestres
    gpio_put(BUZZER, 1);               // Aciona o buzzer
}

// Função para desligar o sinal verde dos pedestres e buzzer
void sinal_vermelho_pedestre() {
    gpio_put(LED_VERDE_PEDRESTES, 0);  // Desliga o LED verde para pedestres
    gpio_put(BUZZER, 0);               // Desliga o buzzer
}

// Função para alternar o semáforo de carros
void semaforo_carros() {
    // Verde para os carros (8 segundos)
    gpio_put(LED_VERDE, 1);
    gpio_put(LED_AMARELO, 0);
    gpio_put(LED_VERMELHO, 0);
    sleep_ms(8000);

    // Amarelo para os carros (2 segundos)
    gpio_put(LED_VERDE, 0);
    gpio_put(LED_AMARELO, 1);
    gpio_put(LED_VERMELHO, 0);
    sleep_ms(2000);

    // Vermelho para os carros (10 segundos)
    gpio_put(LED_VERDE, 0);
    gpio_put(LED_AMARELO, 0);
    gpio_put(LED_VERMELHO, 1);
    sleep_ms(10000);
}

// Função principal
int main() {
    // Inicializa os pinos do Raspberry Pi Pico
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
    gpio_pull_up(BOTAO);  // Configura o botão com resistência de pull-up

    // Inicializa a biblioteca pico/stdlib para uso do sleep_ms
    stdio_init_all();

    while (1) {
        // Funcionamento normal do semáforo
        semaforo_carros();

        // Se o botão for pressionado
        if (gpio_get(BOTAO) == 0) {
            // Sinalizando a travessia dos pedestres
            gpio_put(LED_AMARELO, 1);   // Acende o LED amarelo
            sleep_ms(5000);             // Espera por 5 segundos

            gpio_put(LED_VERMELHO, 1);  // Acende o LED vermelho para carros
            sinal_verde_pedestre();     // Sinal verde para pedestres e buzzer

            sleep_ms(15000);            // Pedestre pode atravessar por 15 segundos

            sinal_vermelho_pedestre();  // Desliga o sinal de pedestres e buzzer
            gpio_put(LED_VERMELHO, 0); // Desliga o LED vermelho para carros

            semaforo_carros();          // Retorna ao ciclo normal dos semáforos
        }
    }

    return 0;
}
