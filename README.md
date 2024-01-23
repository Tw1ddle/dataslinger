[![Dataslinger Logo](https://github.com/Tw1ddle/dataslinger/blob/master/screenshots/logo.png?raw=true "Dataslinger logo")](https://github.com/Tw1ddle/dataslinger)

[![License](https://img.shields.io/badge/License-GPL%20v3-blue.svg?style=flat-square)](https://github.com/Tw1ddle/dataslinger/blob/master/LICENSE)

Demo of [dataslinger-lib](https://github.com/Tw1ddle/dataslinger-lib), a C++ library for conveniently passing data between processes and over a network. I no longer maintain this, don't use it!

## Usage

Download the pre-built demo [here](https://s3.amazonaws.com/dataslinger-bucket/index.html). Open the application and select a demo activity. 

 * Use the sendMessages command in the console to send chunks of data between slingers and receivers.
 * In the image-based activities, try drag-dropping images onto the window and observe the transferred images pop up on the receiving end.

## Building

This is a Qt app written in C++. Requires a compiler with C++17 support, depends on boost 1.78.0+ and a recent version of Qt.

## Screenshots

[![WebSocket implementation client-server](https://github.com/Tw1ddle/dataslinger/blob/master/screenshots/websocket_simple_client_server.png?raw=true "WebSocket simple client-server example")](https://github.com/Tw1ddle/dataslinger)

## Notes
 * Got an idea or suggestion? Open an issue on GitHub, or send Sam a message on [Twitter](https://twitter.com/Sam_Twidale).
