#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"

#define LED_VERMELHO 15
#define LED_AMARELO 14
#define LED_VERDE 13
#define LED_VERDE_PEDESTRES 12
#define BUZZER 21
#define BOTAO 10

// Enum para representar as cores do semáforo
typedef enum {
    VERDE,
    AMARELO,
    VERMELHO,
    VERDE_PEDESTRES
} CorSemaforo;

// Função para acionar o sinal verde dos pedestres e buzzer com frequência ajustável
// Esta função liga o semáforo verde para os pedestres e aciona o buzzer
void sinal_verde_pedestre() {
    ligarSemaforo(VERDE_PEDESTRES);
    ligarBuzzer();
}

// Função para desligar o sinal verde dos pedestres e buzzer
// Esta função desliga o semáforo verde dos pedestres e desliga o buzzer
void desligar_sinal_verde_pedestre() {
    desligarSemaforo(VERDE_PEDESTRES);
    desligarBuzzer();
}

// Função para alternar o semáforo de carros
// O ciclo de semáforo segue a ordem:
// 1. Semáforo verde para os carros (8 segundos)
// 2. Semáforo amarelo para os carros (2 segundos)
// 3. Semáforo vermelho para os carros (10 segundos)
void semaforo_carros() {
    // Verifica se o semáforo de pedestres está desligado para começar a sequência do semáforo de carros
    if (!gpio_get(LED_VERDE_PEDESTRES)) {
        ligarSemaforo(VERDE);  // Liga o semáforo verde para os carros
        verificarBotao(8);  // Espera 8 segundos ou até o botão ser pressionado

        ligarSemaforo(AMARELO);  // Liga o semáforo amarelo para os carros
        verificarBotao(2);  // Espera 2 segundos ou até o botão ser pressionado

        ligarSemaforo(VERMELHO);  // Liga o semáforo vermelho para os carros
        verificarBotao(10);  // Espera 10 segundos ou até o botão ser pressionado
    }
}

// Função para gerenciar o semáforo para pedestres
// Quando o botão é pressionado, esta função inicia a sequência do semáforo para pedestres.
// O semáforo para os carros é desligado antes, e a sequência segue:
// 1. O semáforo amarelo dos carros fica ligado por 5 segundos
// 2. O semáforo vermelho dos carros fica ligado por 15 segundos
// 3. O semáforo verde dos pedestres é ligado por 15 segundos
// Após esse tempo, o semáforo de pedestres e o buzzer é desligado e a sequência dos carros reinicia.
void semaforo_pedestres() {
    printf("Botão pressionado\n");

    // Desligar todos os semáforos dos carros
    desligarSemaforo(VERDE);
    desligarSemaforo(AMARELO);
    desligarSemaforo(VERMELHO);

    // Iniciar sequência de fechamento para os carros
    // O LED amarelo dos carros deve ficar acionado por 5 segundos
    ligarSemaforo(AMARELO);
    sleep_ms(5000);  

    // E depois o vermelho dos carros por 15 segundos
    ligarSemaforo(VERMELHO);
    sleep_ms(15000);  

    // Ligar semáforo verde para pedestres
    sinal_verde_pedestre();

    // O pedestre pode atravessar por 15 segundos
    sleep_ms(15000);  

    // Em seguida, desligar semáforo dos pedestres e buzzer
    desligar_sinal_verde_pedestre();
    desligarSemaforo(VERMELHO);

    // Recomeçar sequência padrão do semáforo dos carros
    semaforo_carros();
}

// Função que espera por um determinado número de segundos ou interrompe se o botão for pressionado
// A cada segundo, verifica se o botão foi pressionado. Se o botão for pressionado antes do 
// tempo limite, a função interrompe o contador e chama a função semaforo_pedestres.
void verificarBotao(int segundos) {
    for (int i = 0; i < segundos; i++) {
        sleep_ms(1000);  // Espera 1 segundo
        if (gpio_get(BOTAO) == 0) {  // Verifica se o botão foi pressionado
            semaforo_pedestres();  // Inicia o semáforo de pedestres
        }
    }
}

// Função para ligar o semáforo de acordo com a cor especificada
// Desliga todos os LEDs antes de ligar o especificado
// Liga o LED correspondente à cor do semáforo informada
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
        case VERDE_PEDESTRES:
            gpio_put(LED_VERDE_PEDESTRES, 1);
            break;
        default:
            printf("Cor inválida\n");
    }
}

// Função para desligar o semáforo de acordo com a cor especificada
// Desliga o LED correspondente à cor do semáforo informada
void desligarSemaforo(CorSemaforo cor) {
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
        case VERDE_PEDESTRES:
            gpio_put(LED_VERDE_PEDESTRES, 0);
            break;
        default:
            printf("Cor inválida\n");
    }
}

// Função para ligar o buzzer com um PWM ajustável
// O buzzer é acionado utilizando controle PWM com frequência ajustável
void ligarBuzzer() {
    gpio_set_function(BUZZER, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(BUZZER);
    pwm_set_clkdiv(slice_num, 500);  // Ajuste o clock para alcançar a frequência
    pwm_set_wrap(slice_num, 2500);      // Definindo o valor de "wrap"
    pwm_set_gpio_level(BUZZER, 500); // Definindo o ciclo de trabalho para o buzzer tocar
    pwm_set_enabled(slice_num, true);   // Habilita o PWM
}

// Função para desligar o buzzer
// Desliga o PWM do buzzer, parando o som
void desligarBuzzer() {
    pwm_set_enabled(pwm_gpio_to_slice_num(BUZZER), false); // Desliga o PWM
}

// Função para executar a inicialização dos pinos do circuito
// Inicializa os pinos dos LEDs, buzzer e botão, configurando suas direções de entrada ou saída
void inicializarPinos() {
    gpio_init(LED_VERMELHO);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);

    gpio_init(LED_AMARELO);
    gpio_set_dir(LED_AMARELO, GPIO_OUT);

    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);

    gpio_init(LED_VERDE_PEDESTRES);
    gpio_set_dir(LED_VERDE_PEDESTRES, GPIO_OUT);

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
        // Verifica se o botão foi pressionado
        if (gpio_get(BOTAO) == 0) {
            sleep_ms(300);  // Debounce
            semaforo_pedestres();
        }

        // Caso contrário, continua a sequência padrão do semáforo dos carros
        semaforo_carros();
    }

    return 0;
}
