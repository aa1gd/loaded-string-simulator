# loaded-string-simulator

loaded-string-simulator is a physics simulator that can simulate loaded strings
and mass-spring coupled oscillators. It uses the GNU Scientific Library (GSL)
for computation and gnuplot to generate plots.

## Compiling

Ensure GSL, libgsl-devel, and gnuplot are installed. Then run

```bash
make
```

## Usage

Setup simulation parameters, following the example in examples/exampleinput.txt

Then run

```bash
./simulate <your_input_file.txt>
```

## Licensing

This project is licensed under the
[GPLv3](https://choosealicense.com/licenses/gpl-3.0/).
