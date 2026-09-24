/* Regras da interface: sem acesso a HTML, localStorage ou arquivos C. */
(function (root) {
  'use strict';
  const MAX_PRODUCTS = 1000;
  const MAX_PRICE_CENTS = 10_000_000;
  const MAX_QUANTITY = 100_000;
  const number = new Intl.NumberFormat('pt-BR');
function validProduct(p) {
  return p && Number.isInteger(p.codigo) && p.codigo > 0 && p.codigo <= 2147483647 &&
    typeof p.nome === 'string' && p.nome.trim().length > 0 && /\p{L}/u.test(p.nome) &&
    !/[\u0000-\u001f\u007f]/.test(p.nome) && new TextEncoder().encode(p.nome).length <= 99 &&
    Number.isInteger(p.quantidade) && p.quantidade >= 0 && p.quantidade <= MAX_QUANTITY &&
    ['precoCompraCentavos', 'precoVendaCentavos'].every(k => Number.isInteger(p[k]) && p[k] >= 0 && p[k] <= MAX_PRICE_CENTS);
}
function validProducts(items) {
  return Array.isArray(items) && items.length <= MAX_PRODUCTS && items.every(validProduct) &&
    new Set(items.map(p => p.codigo)).size === items.length;
}
function parseInteger(value, label, minimum, maximum) {
  if (!/^\d+$/.test(value.trim())) throw new Error(`${label}: digite um número inteiro.`);
  const n = Number(value);
  if (!Number.isSafeInteger(n) || n < minimum || n > maximum) throw new Error(`${label}: use um valor de ${number.format(minimum)} a ${number.format(maximum)}.`);
  return n;
}
function parseCents(value, label) {
  const text = value.trim();
  if (!/^\d+(?:[.,]\d{1,2})?$/.test(text)) throw new Error(`${label}: use um valor positivo ou zero, com até duas casas decimais (ex.: 12,50).`);
  const [whole, fraction = ''] = text.replace(',', '.').split('.');
  const cents = Number(whole) * 100 + Number(fraction.padEnd(2, '0'));
  if (!Number.isSafeInteger(cents) || cents > MAX_PRICE_CENTS) throw new Error(`${label}: o limite é R$ 100.000,00.`);
  return cents;
}

  const api = { MAX_PRODUCTS, MAX_PRICE_CENTS, MAX_QUANTITY, validProduct, validProducts, parseInteger, parseCents };
  if (typeof module !== 'undefined' && module.exports) module.exports = api;
  else root.EstoqueModel = api;
})(globalThis);
