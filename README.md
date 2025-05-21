# SC-proj1
Projeto 1 de Segurança computacional

Esse repositório contém os códigos em C responsáveis pela cifragem e pelo ataque da Cifra de Vigenère. Além disso, eles são acompanhados de um arquivo excel com a fonte do valor utilizado pelo somatório do quadrado das frequências das letras na língua portuguesa. Os dados base para o cálculo foram obtidos da Wikipedia.

A cifra apenas considerará letras para a criptografia e serão tratadas todas elas como minúsculas, outros valores escritos serão apenas ignorados pela cifra, preservados como estão. Logo, é ideal fornecer chaves e mensagens sem caractéres com acentos.
Por padrão, a língua assumida no ataque é inglês, para atacar uma cifra em português, basta trocar o valor de "lang" em vigrack.c para 1
