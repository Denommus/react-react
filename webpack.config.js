const path = require('path');

module.exports = {
  entry: {
    example: './src/example.bs.js',
  },
  output: {
    path: path.join(__dirname, "bundledOutputs"),
    filename: '[name].js',
  },
};
