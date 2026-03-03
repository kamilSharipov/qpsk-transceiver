# PUCCH Format 2 QPSK Transceiver

Реализация блочного кодера/декодера для PUCCH Format 2 в стандартах LTE/5G. Проект моделирует передачу контрольной информации по каналу с шумом, используя блочные коды различных длин (2, 4, 6, 8, 11 бит) и QPSK модуляцию.

## Возможности

- Блочное кодирование/декодирование для длин: 2, 4, 6, 8, 11 бит
- QPSK модуляция и мягкая демодуляция
- Моделирование канала с АБГШ (Additive White Gaussian Noise)
- Три режима работы:
  - `coding` - кодирование информационных бит в QPSK символы
  - `decoding` - декодирование QPSK символов обратно в биты
  - `channel simulation` - симуляция передачи по каналу с расчетом BLER

## Требования

- C++17
- Компилятор: g++ (≥ 11.4.0) или clang (≥ 14.0.0)
- Система сборки: CMake (≥ 3.10)

## Сборка проекта

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make qpsk
```

## Запуск

Программа принимает один аргумент — путь к входному JSON-файлу:

```bash
./qpsk input.json
```

## Бенчмарки декодеров

Проект включает несколько реализаций реализаций декодера:

- `BasicDecoder` - полный перебор всех комбинаций
- `PrecomputedDecoder` - с предвычисленными кодовыми словами
- `SimdDecoder` - AVX2 оптимизированная версия с векторными инструкциями.

### Запуск бенчмарков

```bash
cd build
make benchmark
./benchmark
# или
make run_benchmark
```

## Формат входных данных

Режим `coding`

```json
{
  "mode": "coding",
  "num_of_pucch_f2_bits": 4,
  "pucch_f2_bits": [1, 0, 1, 0]
}
```

Режим `decoding`

```json
{
  "mode": "decoding",
  "num_of_pucch_f2_bits": 4,
  "qpsk_symbols": [
    "1.000000+1.000000j",
    "-1.000000-1.000000j",
    "-1.000000-1.000000j",
    "-1.000000-1.000000j",
    "-1.000000-1.000000j",
    "-1.000000-1.000000j",
    "-1.000000-1.000000j",
    "-1.000000-1.000000j",
    "1.000000+1.000000j",
    "-1.000000-1.000000j"
  ]
}
```

Режим `channel simulation`

```json
{
  "mode": "channel simulation",
  "num_of_pucch_f2_bits": 4,
  "snr_db": 10, 
  "iterations": 1000
}
```

## Формат выходных данных

Режим `coding`

```json
{
  "mode": "coding",
  "qpsk_symbols": [
    "0.707107+0.707107j",
    "-0.707107-0.707107j",
    "-0.707107-0.707107j",
    "-0.707107-0.707107j",
    "-0.707107-0.707107j",
    "-0.707107-0.707107j",
    "-0.707107-0.707107j",
    "-0.707107-0.707107j",
    "0.707107+0.707107j",
    "-0.707107-0.707107j"
  ]
}
```

Режим `decoding`

```json
{
  "mode": "decoding",
  "num_of_pucch_f2_bits": 4,
  "pucch_f2_bits": [1, 0, 1, 0]
}
```

Режим `channel simulation`

```json
{
  "mode": "channel simulation",
  "num_of_pucch_f2_bits": 4,
  "bler": 0.348,
  "success": 652,
  "failed": 348
}
```

## Построение BLER-кривых

Автоматический запуск через CMake
После сборки проекта в директории build доступны цели для построения кривых:

```bash
cd build
make run_plot
```
Настройка параметров скрипта
В файле scripts/plot_bler_curves.py можно изменить:

```python
SNR_VALUES = np.arange(-20, 10, 1.0)   # SNR с шагом
CODE_SIZES = [2, 4, 6, 8, 11]          # Размеры кодов
ITERATIONS = 1000                      # Количество итераций
EXECUTABLE = "./qpsk"                  # Путь к исполняемому файлу
```

## Тестирование

Запуск тестов
```bash
# Сборка с тестами
cmake -B build -DBUILD_TESTING=ON
cmake --build build

# Запуск всех тестов
cd build
ctest

# Запуск с подробным выводом
ctest --output-on-failure
```
