'use strict';

// Primeira etapa: interface independente do backend C.
// Quando a API existir, substitua carregarProdutos/salvarProdutos por chamadas fetch.
const STORAGE_KEY = 'controle-vendas.frontend.v1';
const { MAX_PRODUCTS, MAX_QUANTITY, validProducts, parseInteger, parseCents } = EstoqueModel;
const exampleProducts = [
  { codigo: 101, nome: 'Água mineral 500 ml', precoCompraCentavos: 150, precoVendaCentavos: 300, quantidade: 24 },
  { codigo: 102, nome: 'Arroz branco 1 kg', precoCompraCentavos: 420, precoVendaCentavos: 650, quantidade: 18 },
  { codigo: 103, nome: 'Café torrado 500 g', precoCompraCentavos: 1450, precoVendaCentavos: 1990, quantidade: 4 },
  { codigo: 104, nome: 'Leite integral 1 L', precoCompraCentavos: 380, precoVendaCentavos: 550, quantidade: 32 },
  { codigo: 105, nome: 'Óleo de soja 900 ml', precoCompraCentavos: 590, precoVendaCentavos: 790, quantidade: 3 },
  { codigo: 106, nome: 'Açúcar refinado 1 kg', precoCompraCentavos: 310, precoVendaCentavos: 490, quantidade: 0 },
  { codigo: 107, nome: 'Feijão carioca 1 kg', precoCompraCentavos: 620, precoVendaCentavos: 890, quantidade: 16 },
  { codigo: 108, nome: 'Macarrão espaguete 500 g', precoCompraCentavos: 280, precoVendaCentavos: 450, quantidade: 8 },
];
const icons = {
  box: '<path d="m12 3 9 5-9 5-9-5 9-5Z"/><path d="M3 8v9l9 5 9-5V8M12 13v9M7.5 5.5l9 5"/>',
  shop: '<path d="M3 10 5 4h14l2 6M4 13v8h16v-8M9 21v-7h6v7"/><path d="M3 10a3 3 0 0 0 6 0 3 3 0 0 0 6 0 3 3 0 0 0 6 0H3Z"/>',
  grid: '<rect x="3" y="3" width="7" height="7" rx="1.5"/><rect x="14" y="3" width="7" height="7" rx="1.5"/><rect x="3" y="14" width="7" height="7" rx="1.5"/><rect x="14" y="14" width="7" height="7" rx="1.5"/>',
  help: '<circle cx="12" cy="12" r="9"/><path d="M9.4 9a2.7 2.7 0 0 1 5.2 1c0 2-2.6 2-2.6 4M12 17h.01"/>',
  layers: '<path d="m12 3 9 5-9 5-9-5 9-5ZM3 12l9 5 9-5M3 16l9 5 9-5"/>',
  wallet: '<path d="M20 8H5a2 2 0 0 1 0-4h13v4M3 6v13a2 2 0 0 0 2 2h15V8"/><path d="M20 12h-5v5h5M16.8 14.5h.1"/>',
  plus: '<path d="M12 5v14M5 12h14"/>',
  info: '<circle cx="12" cy="12" r="9"/><path d="M12 11v6M12 7h.01"/>',
  search: '<circle cx="10.5" cy="10.5" r="6.5"/><path d="m16 16 5 5"/>',
  download: '<path d="M12 3v12m-4-4 4 4 4-4M4 16v5h16v-5"/>',
  close: '<path d="m6 6 12 12M6 18 18 6"/>',
  edit: '<path d="m15 4 5 5M4 20l5-1L21 7a2 2 0 0 0-5-5L4 14v6Z"/>',
};
const $ = id => document.getElementById(id);
const currency = new Intl.NumberFormat('pt-BR', { style: 'currency', currency: 'BRL' });
const number = new Intl.NumberFormat('pt-BR');
const money = cents => currency.format(cents / 100);
const normalize = text => String(text).normalize('NFD').replace(/[\u0300-\u036f]/g, '').toLocaleLowerCase('pt-BR');
const cloneExamples = () => exampleProducts.map(p => ({ ...p }));
let currentFilter = 'all';
let editingCode = null;
let memoryOnly = false;
let damagedStorage = false;
let toastTimer;

function icon(name) {
  const span = document.createElement('span');
  // Apenas SVGs constantes da aplicação; dados de produtos usam textContent.
  span.innerHTML = `<svg viewBox="0 0 24 24" aria-hidden="true">${icons[name] || icons.box}</svg>`;
  return span;
}
document.querySelectorAll('[data-icon]').forEach(el => el.append(icon(el.dataset.icon).firstChild));

function warnStorage(message) {
  $('storage-warning').textContent = message;
  $('storage-warning').hidden = false;
}
function carregarProdutos() {
  let raw;
  try { raw = localStorage.getItem(STORAGE_KEY); }
  catch {
    memoryOnly = true;
    warnStorage('O navegador bloqueou o armazenamento. As alterações durarão apenas nesta sessão; exporte uma cópia para guardá-las.');
    return cloneExamples();
  }
  if (raw === null) return cloneExamples();
  try {
    const data = JSON.parse(raw);
    if (data.version !== 1 || !validProducts(data.produtos)) throw new Error('Invalid data');
    return data.produtos;
  } catch {
    damagedStorage = true;
    warnStorage('Os dados salvos não puderam ser lidos. Eles foram preservados. O cadastro está bloqueado até você escolher “Restaurar dados de exemplo”.');
    return [];
  }
}
let products = carregarProdutos();

function salvarProdutos(next, allowReset = false) {
  if (!validProducts(next)) throw new Error('Dados de produtos inválidos.');
  if (damagedStorage && !allowReset) throw new Error('Restaure os exemplos antes de cadastrar novos produtos.');
  if (!memoryOnly) {
    try { localStorage.setItem(STORAGE_KEY, JSON.stringify({ version: 1, produtos: next })); }
    catch { throw new Error('Não foi possível salvar. O limite do navegador pode ter sido atingido. Nenhuma alteração foi aplicada.'); }
  }
  products = next;
  damagedStorage = false;
  if (!memoryOnly) $('storage-warning').hidden = true;
}
function notify(message) {
  clearTimeout(toastTimer);
  $('toast').textContent = message;
  $('toast').hidden = false;
  toastTimer = setTimeout(() => { $('toast').hidden = true; }, 4500);
}
function element(tag, className, text) {
  const el = document.createElement(tag);
  if (className) el.className = className;
  if (text !== undefined) el.textContent = text;
  return el;
}
function render() {
  const low = products.filter(p => p.quantidade > 0 && p.quantidade <= 5).length;
  $('metric-products').textContent = number.format(products.length);
  $('metric-units').textContent = number.format(products.reduce((s, p) => s + p.quantidade, 0));
  $('metric-value').textContent = money(products.reduce((s, p) => s + p.quantidade * p.precoCompraCentavos, 0));
  $('nav-count').textContent = products.length;
  $('catalog-count').textContent = products.length;
  $('low-count').textContent = low;
  $('add-button').disabled = damagedStorage || products.length >= MAX_PRODUCTS;
  $('export-button').disabled = damagedStorage;
  const query = normalize($('search').value.trim());
  const visible = products.filter(p => {
    const matches = normalize(p.nome).includes(query) || String(p.codigo).includes(query);
    return matches && (currentFilter === 'all' || (currentFilter === 'low' && p.quantidade > 0 && p.quantidade <= 5) || (currentFilter === 'empty' && p.quantidade === 0));
  }).sort((a, b) => a.codigo - b.codigo);
  const body = $('product-rows');
  body.replaceChildren();
  visible.forEach(p => {
    const tr = document.createElement('tr');
    const cell = document.createElement('td');
    const product = element('div', 'product-cell');
    const initials = p.nome.split(/\s+/).slice(0, 2).map(w => [...w][0]).join('').toLocaleUpperCase('pt-BR');
    const avatar = element('span', `product-avatar tone-${p.codigo % 4}`, initials);
    avatar.setAttribute('aria-hidden', 'true');
    product.append(avatar, element('span', '', p.nome)); cell.append(product); tr.append(cell);
    tr.append(element('td', 'product-code', `#${p.codigo}`), element('td', 'number', money(p.precoCompraCentavos)), element('td', 'number', money(p.precoVendaCentavos)), element('td', 'number quantity', number.format(p.quantidade)));
    const state = p.quantidade === 0 ? ['empty', 'Sem estoque'] : p.quantidade <= 5 ? ['low', 'Estoque baixo'] : ['ok', 'Disponível'];
    const badgeCell = document.createElement('td'); badgeCell.append(element('span', `badge ${state[0]}`, state[1])); tr.append(badgeCell);
    const actionCell = document.createElement('td');
    const edit = element('button', 'icon-button');
    edit.type = 'button'; edit.setAttribute('aria-label', `Editar ${p.nome}`); edit.title = 'Editar produto'; edit.append(icon('edit')); edit.addEventListener('click', () => openForm(p));
    actionCell.append(edit); tr.append(actionCell); body.append(tr);
  });
  $('empty-state').hidden = visible.length !== 0;
  $('result-count').textContent = `Mostrando ${visible.length} de ${products.length} produtos`;
  document.querySelectorAll('[data-filter]').forEach(button => {
    const active = button.dataset.filter === currentFilter;
    button.classList.toggle('active', active); button.setAttribute('aria-pressed', String(active));
  });
}
function openForm(product = null) {
  const form = $('product-form');
  form.reset(); editingCode = product?.codigo ?? null;
  $('form-error').hidden = true;
  $('dialog-title').textContent = product ? 'Editar produto' : 'Novo produto';
  $('save-button').textContent = product ? 'Salvar alterações' : 'Cadastrar produto';
  form.elements.codigo.value = product ? product.codigo : nextCode();
  form.elements.quantidade.value = product?.quantidade ?? 0;
  if (product) {
    form.elements.nome.value = product.nome;
    form.elements.compra.value = (product.precoCompraCentavos / 100).toFixed(2).replace('.', ',');
    form.elements.venda.value = (product.precoVendaCentavos / 100).toFixed(2).replace('.', ',');
  }
  $('product-dialog').showModal();
  $('field-name').focus();
}
function nextCode() {
  const codes = new Set(products.map(p => p.codigo));
  let next = 1;
  while (codes.has(next)) next++;
  return next;
}
$('product-form').addEventListener('submit', event => {
  event.preventDefault();
  const form = event.currentTarget;
  try {
    const nome = form.elements.nome.value.trim().replace(/\s+/g, ' ');
    if (!/\p{L}/u.test(nome) || /[\u0000-\u001f\u007f]/.test(nome) || new TextEncoder().encode(nome).length > 99) throw new Error('Digite um nome com letras e até 99 bytes em UTF-8.');
    const product = {
      nome,
      codigo: parseInteger(form.elements.codigo.value, 'Código', 1, 2147483647),
      quantidade: parseInteger(form.elements.quantidade.value, 'Quantidade', 0, MAX_QUANTITY),
      precoCompraCentavos: parseCents(form.elements.compra.value, 'Preço de compra'),
      precoVendaCentavos: parseCents(form.elements.venda.value, 'Preço de venda'),
    };
    if (products.some(p => p.codigo === product.codigo && p.codigo !== editingCode)) throw new Error('Já existe um produto com esse código. Escolha outro.');
    const next = editingCode === null ? [...products, product] : products.map(p => p.codigo === editingCode ? product : p);
    salvarProdutos(next);
    currentFilter = 'all'; $('search').value = ''; render();
    $('product-dialog').close();
    notify(memoryOnly ? 'Produto atualizado apenas nesta sessão.' : editingCode === null ? 'Produto cadastrado neste navegador.' : 'Alterações salvas neste navegador.');
  } catch (error) {
    $('form-error').textContent = error.message; $('form-error').hidden = false;
  }
});
$('add-button').addEventListener('click', () => openForm());
$('search').addEventListener('input', render);
document.querySelectorAll('[data-filter]').forEach(button => button.addEventListener('click', () => { currentFilter = button.dataset.filter; render(); }));
$('clear-button').addEventListener('click', () => { currentFilter = 'all'; $('search').value = ''; render(); $('search').focus(); });
document.querySelectorAll('[data-close]').forEach(button => button.addEventListener('click', () => $(button.dataset.close).close()));
$('about-button').addEventListener('click', () => $('about-dialog').showModal());
$('about-mobile').addEventListener('click', () => $('about-dialog').showModal());
$('reset-button').addEventListener('click', () => $('reset-dialog').showModal());
$('confirm-reset').addEventListener('click', () => {
  try {
    salvarProdutos(cloneExamples(), true); currentFilter = 'all'; $('search').value = ''; render();
    $('reset-dialog').close(); notify('Os dados de exemplo foram restaurados.');
  } catch (error) { $('reset-dialog').close(); warnStorage(error.message); }
});
$('export-button').addEventListener('click', () => {
  const data = { version: 1, exportedAt: new Date().toISOString(), produtos: products };
  const blob = new Blob([JSON.stringify(data, null, 2)], { type: 'application/json;charset=utf-8' });
  const url = URL.createObjectURL(blob);
  const link = document.createElement('a'); link.href = url; link.download = 'produtos-frontend.json';
  document.body.append(link); link.click(); link.remove(); setTimeout(() => URL.revokeObjectURL(url), 1000);
  notify('Cópia JSON exportada.');
});
render();
$('pdf-button').addEventListener('click', () => {
  // Print the complete catalog, regardless of the current search.
  const previousFilter = currentFilter;
  const previousSearch = $('search').value;

  currentFilter = 'all';
  $('search').value = '';
  render();

  const date = new Date().toLocaleString('pt-BR');

  $('print-date').textContent = `Emitido em: ${date}`;

  // Restore the user's search and filter after printing or cancelling.
  function restoreScreen() {
    currentFilter = previousFilter;
    $('search').value = previousSearch;
    render();
  }

  window.addEventListener('afterprint', restoreScreen, { once: true });

  window.print();
});
