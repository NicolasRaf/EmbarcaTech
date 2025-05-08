  const express = require('express');
  const http = require('http');
  const WebSocket = require('ws');
  const app = express();
  const port = process.env.PORT || 3000;

  app.use(express.json());

  const server = http.createServer(app);
  const wss = new WebSocket.Server({ server });

  // Armazena os clientes WebSocket conectados
  let clients = [];

  // Conexões WebSocket
  wss.on('connection', (ws) => {
    // Novo cliente conectado
    console.log('🟢 Novo cliente WebSocket conectado.');
    clients.push(ws);

    // Evento de desconexão
    ws.on('close', () => {
      // Remove o cliente da lista de clientes conectados
      clients = clients.filter(c => c !== ws);
      console.log('🔴 Cliente WebSocket desconectado.');
    });
  });

  // Rota para receber dados 
  app.post('/update', (req, res) => {
    const data = req.body;
    console.log('\n' + '='.repeat(20) + '\nDados recebidos:\n', data);

    // Envia para todos os clientes conectados via WebSocket
    clients.forEach(ws => {
      // Verifica se o cliente está conectado e pronto para receber dados
      if (ws.readyState === WebSocket.OPEN) {
        // Envia os dados recebidos para o cliente via WebSocket
        ws.send(JSON.stringify(data));
      }
    });

    // Retorna uma resposta HTTP com status 200
    res.status(200).send('Dados recebidos com sucesso!');
  });

  // Iniciar servidor
  server.listen(port, () => {
    console.log(`🚀 Servidor rodando na porta ${port}`);  
  });
  