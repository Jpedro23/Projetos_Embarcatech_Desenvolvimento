# Importa as bibliotecas Flask e SocketIO
from flask import Flask, render_template, request, jsonify
from flask_socketio import SocketIO, emit

# Cria a instância do Flask
app = Flask(__name__)

# Configura o SocketIO para permitir conexões de qualquer origem
socketio = SocketIO(app, cors_allowed_origins="*")

# Armazena a última mensagem de temperatura (evita sobrescrever lista)
mensagem_temperatura = None

# Rota principal que serve a página HTML
@app.route('/')
def index():
    return render_template('index.html')

# ------------------------------------- Rota para a temperatura -------------------------------------
@app.route('/temperature', methods=['GET'])
def temperature():
    global mensagem_temperatura
    temperature = request.args.get('value')

    if temperature:
        try:
            temperature_value = float(temperature)
        except ValueError:
            return jsonify({"status": "erro", "mensagem": "Temperatura inválida"}), 400

        nova_mensagem = f"{temperature_value:.2f}°C"  

        # Apenas atualiza se for diferente da última
        if mensagem_temperatura != nova_mensagem:
            mensagem_temperatura = nova_mensagem
            print(f"\n ---------------------------------------- \n Nova temperatura registrada:, {mensagem_temperatura}\n ---------------------------------------- \n ")

        return jsonify([f"Temperatura lida: {temperature_value:.2f}°C"]), 200


    return jsonify({"status": "erro", "mensagem": "Nenhuma temperatura fornecida"}), 400

# ------------------------------------- Nova rota para fornecer a última temperatura registrada -------------------------------------
@app.route('/ultima_temperatura', methods=['GET'])
def ultima_temperatura():
    global mensagem_temperatura
    if mensagem_temperatura:
        print(f' \n ---------------------------------------- \n Ultima Temperatura lida, {mensagem_temperatura} \n ---------------------------------------- \n ')
        return jsonify([mensagem_temperatura]), 200
    else:
        return jsonify(["Nenhuma temperatura registrada ainda"]), 200


# ------------------------------------- Rota sobre as direções -------------------------------------

#------------------------------------- Nova rota para retornar a última direção (para uso pelo front-end) -------------------------------------
direcao_atual = "centro"  # Começa sem direção definida

@app.route('/direcao', methods=['GET'])
def obter_direcao():
    global direcao_atual
    return jsonify({"direcao": direcao_atual})

# Envia a direção para o cliente via Socket.IO quando a direção for recebida
@app.route('/direction', methods=['GET'])
def direction():
    global direcao_atual
    direction = request.args.get('dir')

    if not direction or direction.lower() not in ['norte', 'sul', 'leste', 'oeste', 'sudeste', 'sudoeste', 'nordeste', 'noroeste', 'centro']:
        return jsonify({'erro': 'Direção inválida'}), 400

    print(f"\n -------------------------------- \n Direção recebida: {direction}\n -------------------------------- \n")
    direcao_atual = direction.lower()

    socketio.emit('nova_direcao', {'direction': direction})

    return jsonify({'status': 'ok', 'direction': direction})

# ------------------------------------- Rota de estado do botão -------------------------------------

@app.route('/pressed_button_a', methods=['GET','POST'])
def pressed_button_a():
    print("Comando: butão_a_precionado")
    socketio.emit('command', {'action':'pressed_button_a'}) 
    return 'pressed_button_a command sent', 200

@app.route('/pressed_button_b', methods=['GET','POST'])
def pressed_button_b():
    print("Comando: butão_b_precionado")
    socketio.emit('command', {'action':'pressed_button_b'}) 
    return 'pressed_button_b command sent', 200

@app.route('/unpressed', methods=['GET','POST'])
def unpressed():
    print("\n------------- Nenhum botão pressionado -------------\n")
    socketio.emit('command',{'action':'unpressed'})
    return 'unpressed command sent',200
    
# Ponto de entrada principal da aplicação
if __name__ == '__main__':
    socketio.run(app, host='0.0.0.0', port=5000)
