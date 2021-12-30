const path = require("path")
const HtmlWebpackPlugin = require("html-webpack-plugin")
const outputDir = path.join(__dirname, "bundledOutputs/")

const isProd = process.env.NODE_ENV === "production"

module.exports = {
  mode: isProd ? "production" : "development",
  devtool: "source-map",
  entry: {
    example: './src/example.bs.js',
  },
  output: {
    path: outputDir,
    filename: '[name].js',
  },
  plugins: [
    new HtmlWebpackPlugin({
      template: "public/index.html",
      inject: false
    })
  ],
  devServer: {
    compress: true,
    port: process.env.PORT || 8000,
    historyApiFallback: true
  }
}
