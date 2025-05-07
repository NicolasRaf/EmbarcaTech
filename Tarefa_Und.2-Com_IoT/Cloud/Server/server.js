const express = require('express');
const app = express();
const port = 3000;

// Middleware para interpretar JSON
app.use(express.json());

// Rota POST para /update
app.post('/update', (req, res) => {
  const data = req.body;

  console.log('\n'+'='.repeat(20)+'\nDados recebidos:');
  console.log(JSON.stringify(data, null, 2));

  // Aqui você poderia salvar em banco, emitir alertas, etc.
  res.status(200).send('Dados recebidos com sucesso!');
});

// Iniciar servidor
app.listen(port, () => {
  console.log(`🚀 Servidor rodando em http://localhost:${port}`);
});
