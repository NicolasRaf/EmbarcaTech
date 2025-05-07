const express = require('express');
const http = require('http');
const WebSocket = require('ws');
const app = express();
const port = 3000;

app.use(express.json());

const server = http.createServer(app);
const wss = new WebSocket.Server({ server });

let clients = [];

// Conexões WebSocket
wss.on('connection', (ws) => {
  console.log('🟢 Novo cliente WebSocket conectado.');
  clients.push(ws);

  ws.on('close', () => {
    clients = clients.filter(c => c !== ws);
    console.log('🔴 Cliente WebSocket desconectado.');
  });
});

// Rota para receber dados do ESP32
app.post('/update', (req, res) => {
  const data = req.body;
  console.log('\n' + '='.repeat(20) + '\nDados recebidos:\n', data);

  // Envia para todos os clientes conectados via WebSocket
  clients.forEach(ws => {
    if (ws.readyState === WebSocket.OPEN) {
      ws.send(JSON.stringify(data));
    }
  });

  res.status(200).send('Dados recebidos com sucesso!');
});

// Iniciar servidor
server.listen(port, () => {
  console.log(`🚀 Servidor rodando em http://localhost:${port}`);
});