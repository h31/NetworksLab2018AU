# 1. communication format

## 1.1. protocol description

### 1.1.1. abstract
This is a binary protocol, that have been developed specifically
for the simple messenger application. It works in network byte order
(i.e. big-endian) and uses the same format for input and output messages.

### 1.1.2. characteristics

#### 1.1.2.1. byte order
It uses big-endian byte order.

#### 1.1.2.2. binarity
This is a binary protocol, which supports only ascii strings.

### 1.1.3. message format

#### 1.1.3.1. preambule

Preambule consists of a name of the message author.
It's encoded as an 32-bit unsigned integer <name_size>, which represents size of the name,
and following ascii string without terminator of <name_size> symbols.

#### 1.1.3.2. body

As the name, the message body is represented by a 32-bit unsigned integer <body_size> and
following <body_size> bytes, that is a text of the sent message.