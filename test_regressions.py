"""Black-box regression tests; all data is created in temporary directories."""
import os
import pathlib
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[1]
SEED = '1 Agua 2.00 5.00 10\n2 Arroz 4.00 7.00 20\n'

class Regressions(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.build = tempfile.TemporaryDirectory()
        cls.exe = str(pathlib.Path(cls.build.name) / 'loja')
        subprocess.run(['gcc', '-std=c11', '-Wall', '-Wextra', '-Wpedantic', '-Werror',
                        *map(str, sorted((ROOT / 'Backend').glob('*.c'))), '-lm', '-o', cls.exe], check=True)
    @classmethod
    def tearDownClass(cls):
        cls.build.cleanup()
    def run_case(self, inputs, seed=SEED, extra=None, directories=()):
        with tempfile.TemporaryDirectory() as d:
            p = pathlib.Path(d)
            if seed is not None: (p / 'produtos.txt').write_text(seed)
            for name, content in (extra or {}).items(): (p / name).write_text(content)
            for name in directories: (p / name).mkdir()
            r = subprocess.run([self.exe], input=inputs, cwd=d, capture_output=True, text=True, timeout=3)
            self.assertEqual(r.returncode, 0, r.stderr)
            files = {f.name: f.read_text() for f in p.glob('*.txt') if f.is_file()}
            return r.stdout, files
    def test_normal_sale(self):
        out, f = self.run_case('6\n1\n3\n0\n')
        self.assertIn('1 Agua 2.00 5.00 7\n', f['produtos.txt'])
        self.assertEqual(f['vendas.txt'], '1 1 3 15.00 6.00 9.00\n')
        self.assertIn('registrada com sucesso', out)
    def test_negative_and_zero_sale_retry(self):
        out, f = self.run_case('6\n1\n-3\n0\n2\n0\n')
        self.assertEqual(out.count('A quantidade deve ser maior'), 2)
        self.assertIn('1 Agua 2.00 5.00 8', f['produtos.txt'])
        self.assertEqual(f['vendas.txt'], '1 1 2 10.00 4.00 6.00\n')
    def test_negative_loss_retry(self):
        _, f = self.run_case('10\n1\n-3\n0\n2\n0\n')
        self.assertEqual(f['perdas.txt'], '1 1 2 4.00\n')
        self.assertIn('1 Agua 2.00 5.00 8', f['produtos.txt'])
    def test_insufficient_stock(self):
        _, f = self.run_case('6\n1\n11\n10\n1\n11\n0\n')
        self.assertEqual(f['produtos.txt'], SEED)
        self.assertNotIn('vendas.txt', f)
        self.assertNotIn('perdas.txt', f)
    def test_edit_name_then_delete_missing_preserves_products(self):
        _, f = self.run_case('4\n1\nAgua quente\n2\n5\n10\n5\n999\n0\n')
        self.assertEqual(f['produtos.txt'], SEED.replace('Agua', 'Agua_quente'))
    def test_tab_name(self):
        _, f = self.run_case('4\n1\nAgua\tquente\n2\n5\n10\n0\n')
        self.assertIn('Agua_quente', f['produtos.txt'])
    def test_invalid_edit_price_retries(self):
        _, f = self.run_case('4\n1\nAgua\nabc\n3\n6\n10\n0\n')
        self.assertIn('1 Agua 3.00 6.00 10', f['produtos.txt'])
    def test_register_validated_fields(self):
        _, f = self.run_case('1\n3\nLeite\nabc\nnan\ninf\n-2\n2,50\nno\n3.50\nbad\n-1\n4\n0\n')
        self.assertIn('3 Leite 2.50 3.50 4', f['produtos.txt'])
    def test_duplicate_registration(self):
        _, f = self.run_case('1\n1\n3\nLeite\n2\n3\n4\n0\n')
        self.assertEqual(f['produtos.txt'].count('1 Agua'), 1)
        self.assertIn('3 Leite', f['produtos.txt'])
    def test_long_name_does_not_eat_next_input(self):
        _, f = self.run_case('1\n3\n'+'a'*100+'\nLeite\n2\n3\n4\n0\n')
        self.assertIn('3 Leite', f['produtos.txt'])
    def test_overlong_input_is_drained(self):
        _, f = self.run_case('1\n'+'9'*500+'\n3\n'+'a'*700+'\nLeite\n2\n3\n4\n0\n')
        self.assertIn('3 Leite', f['produtos.txt'])
    def test_eof_menu(self):
        out, f = self.run_case('')
        self.assertIn('Entrada encerrada', out)
        self.assertEqual(f['produtos.txt'], SEED)
    def test_eof_during_edit_preserves_file(self):
        _, f = self.run_case('4\n1\nAgua')
        self.assertEqual(f['produtos.txt'], SEED)
    def test_corrupt_products_never_truncated(self):
        bad = SEED + '3 broken record\n'
        for operation in ['5\n1\n0\n', '4\n1\n0\n', '6\n1\n2\n0\n', '10\n1\n2\n0\n', '1\n3\nLeite\n2\n3\n4\n0\n']:
            with self.subTest(operation=operation):
                _, f = self.run_case(operation, seed=bad)
                self.assertEqual(f['produtos.txt'], bad)
                self.assertNotIn('vendas.txt', f)
                self.assertNotIn('perdas.txt', f)
    def test_duplicate_codes_in_file_block_sale(self):
        bad = SEED + '1 Outro 1.00 2.00 4\n'
        _, f = self.run_case('6\n1\n1\n0\n', seed=bad)
        self.assertEqual(f['produtos.txt'], bad)
        self.assertNotIn('vendas.txt', f)
    def test_invalid_histories_block_sale(self):
        for bad in ['1 1 3\n', '1 1 3', '1 1 -3 -15 -6 -9\n', '1 1 1 nan 2 3\n', '9999999999999999999999 1 1 5 2 3\n']:
            with self.subTest(history=bad):
                _, f = self.run_case('6\n1\n1\n0\n', extra={'vendas.txt': bad})
                self.assertEqual(f['produtos.txt'], SEED)
                self.assertEqual(f['vendas.txt'], bad)
    def test_cannot_open_history_preserves_stock(self):
        _, f = self.run_case('6\n1\n1\n0\n', directories=['vendas.txt'])
        self.assertEqual(f['produtos.txt'], SEED)
    def test_cannot_create_temp_preserves_stock(self):
        _, f = self.run_case('5\n1\n0\n', directories=['temp.txt'])
        self.assertEqual(f['produtos.txt'], SEED)
    def test_empty_install_registration(self):
        _, f = self.run_case('1\n1\nAgua quente\n2\n5\n10\n0\n', seed=None)
        self.assertEqual(f['produtos.txt'], '1 Agua_quente 2.00 5.00 10\n')
    def test_reports(self):
        out, _ = self.run_case('6\n1\n2\n10\n1\n1\n7\n8\n9\n11\n0\n')
        self.assertIn('Faturamento total: R$ 10.00', out)
        self.assertIn('Resultado apos perdas: R$ 4.00', out)
    def test_second_process_is_blocked(self):
        with tempfile.TemporaryDirectory() as d:
            first = subprocess.Popen([self.exe], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd=d, text=True)
            try:
                # Wait for the first process's first menu line: lock is already held.
                first.stdout.readline()
                second = subprocess.run([self.exe], input='0\n', cwd=d, capture_output=True, text=True, timeout=3)
                self.assertNotEqual(second.returncode, 0)
                self.assertIn('Outro processo', second.stderr)
            finally:
                first.communicate('0\n', timeout=3)

if __name__ == '__main__': unittest.main(verbosity=2)
