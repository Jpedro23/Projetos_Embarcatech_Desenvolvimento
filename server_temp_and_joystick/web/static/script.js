let currentAngle = 0;

// 🔄 Atualiza o ponteiro da bússola com base em um ângulo
function updatePointer(angle) {
    currentAngle = angle;

    if (currentAngle < 0) currentAngle += 360;
    if (currentAngle >= 360) currentAngle %= 360;

    const compassPointer = document.getElementById('compassPointer');
    const angleDisplay = document.getElementById('angleDisplay');

    if (compassPointer && angleDisplay) {
        compassPointer.style.transform = `translateX(-50%) translateY(-100%) rotate(${currentAngle}deg)`;
        angleDisplay.textContent = `Ângulo: ${Math.round(currentAngle)}°`;
    }
}

// 🧭 Atualiza o ponteiro com base na direção textual
function updatePointerFromDirection(direcao) {
    const direcoes = {
        "norte": 0,
        "nordeste": 45,
        "leste": 90,
        "sudeste": 135,
        "sul": 180,
        "sudoeste": 225,
        "oeste": 270,
        "noroeste": 315,
        "centro": 0
    };

    const angulo = direcoes[direcao.toLowerCase()];
    if (angulo !== undefined) {
        updatePointer(angulo);
    } else {
        console.error('Direção inválida:', direcao);
    }
}

document.addEventListener('DOMContentLoaded', function () {
    const temperatureElement = document.getElementById('temperature');
    const lastUpdateElement = document.getElementById('last-update');
    const statusMessageElement = document.getElementById('status-message');
    const temperatureContainer = document.getElementById('temperature-container');
    const rotateLeftBtn = document.getElementById('rotateLeft');
    const rotateRightBtn = document.getElementById('rotateRight');
    const resetPointerBtn = document.getElementById('resetPointer');

    // 🌡️ Atualiza temperatura
    async function atualizarTemperatura() {
        try {
            statusMessageElement.textContent = "Atualizando temperatura...";

            const response = await fetch('/ultima_temperatura');
            const data = await response.json();

            if (Array.isArray(data) && data.length > 0) {
                const temperatura = data[0];
                temperatureElement.textContent = `${temperatura}`;

                temperatureContainer.classList.add('updating');
                setTimeout(() => {
                    temperatureContainer.classList.remove('updating');
                }, 1000);

                const now = new Date();
                lastUpdateElement.textContent = now.toLocaleTimeString('pt-BR');
                statusMessageElement.textContent = "Sensor conectado";
            } else {
                statusMessageElement.textContent = "Dados inválidos recebidos do sensor.";
            }
        } catch (error) {
            statusMessageElement.textContent = "Erro ao conectar com o sensor. Tentando novamente...";
            console.error("Erro ao atualizar temperatura:", error);
        }
    }

    atualizarTemperatura();
    setInterval(atualizarTemperatura, 3000);

    temperatureContainer.addEventListener('click', () => {
        atualizarTemperatura();
        statusMessageElement.textContent = "Atualizando manualmente...";
    });

    // 🧭 Atualiza rosa dos ventos
    async function atualizarRosa() {
        try {
            const res = await fetch('/direcao');
            const data = await res.json();
            console.log(data.direcao);
            updatePointerFromDirection(data.direcao);
        } catch (err) {
            console.error("Erro ao buscar direção:", err);
        }
    }

    setInterval(atualizarRosa, 100);

    resetPointerBtn.addEventListener("click", async () => {
        try {
            await fetch("/direction?dir=norte");
            //Força o update visual imediato
            updatePointerFromDirection("norte");
        } catch (err) {
            //Retorna uma mensagem caso não seja possivel resetar a direção
            console.error("Erro ao resetar direção:", err);
        }
    });


});




// 🔌 Conecta com o servidor via WebSocket
const socket = io();

socket.on('nova_direcao', function (data) {
    updatePointerFromDirection(data.direction);
});

// Exeuta o sistema de verificação de estado do botão a e botão b

const physicalButton = document.getElementById('physicalButton');

function atualizarStatus(texto) {
  physicalButton.textContent = texto;
}

document.addEventListener('keydown', (e) => {
  if (e.key === 'ArrowLeft') atualizarStatus('Seta Esquerda Pressionada');
});


socket.on('command', (data) => {
  if (data.action === 'pressed_button_a') atualizarStatus('Butão_A_Pressionado'); 
  if (data.action === 'pressed_button_b') atualizarStatus('Butão_B_Pressionado'); 
  if (data.action === 'unpressed') atualizarStatus('Solto'); 
});