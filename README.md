# Shearsort 
 A distributed/parallel sorting algorithm for 2-dimensional arrays of size n by n
## Usage
  - complile with `gcc shearsort.c -pthread` on any terminal
  - run `./a.out [textfile where array data is stored]`
  - textfile must contain length of array matrix (so n = 4 for a array of size 16) on first line, followed by array (n integers seperated by a space on each line)

## Example

### Input

```
4
3 11 6 16
8 1 5 10
14 7 12 2
4 13 9 15
```

### Output

```
Initial Mesh Array

3 11 6 16
8 1 5 10
14 7 12 2
4 13 9 15

After Phase 1

3 6 11 16
10 8 5 1
2 7 12 14
15 13 9 4

After Phase 2

2 6 5 1
3 7 9 4
10 8 11 14
15 13 12 16

After Phase 3

1 2 5 6
9 7 4 3
8 10 11 14
16 15 13 12

After Phase 4

1 2 4 3
8 7 5 6
9 10 11 12
16 15 13 14

After Phase 5

1 2 3 4
8 7 6 5
9 10 11 12
16 15 14 13
```

## Todos
  - Use better sorting algorithm for worker threads (currently using bubble sort)
  - make file

## Future Ideas
  - move to message passing concurrency and try to make algorithm distributed (running it on multiple machines). Challenges:
    - Lots of data to share between channels.
    - entry/exit of machines during running process (what if worker thread dies? nhow to redistribute work?)
    