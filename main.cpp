#include <iostream>
#include <cmath>
#include <cassert>
#include "bdd.h"

enum Neighbours
{
  DOWN,
  RIGHT_DOWN
};

const int N = 9;
const int M = 4;
const int N_SQRT = sqrt(N);
const int LOG_N = static_cast<int>(std::ceil(std::log2(N)));;

const int N_VAR = N * M * LOG_N;
char* var;

bdd limitation_1(bdd p[M][N][N], int propertyNumber, int objectNumber, int value);
bdd limitation_2(bdd p[M][N][N], int number1, int value1, int number2, int value2);
bdd limitation_3(bdd p[M][N][N], Neighbours neighbour, int number1, int value1, int number2, int value2);
bdd limitation_4(bdd p[M][N][N], int number1, int value1, int number2, int value2);
void limitation_5(bdd& tree, bdd p[M][N][N]);
bdd limitation_6(bdd p[M][N][N]);

void init(bdd p[M][N][N]);

void fun(char* varset, int size);

int main()
{
  var = new char[N_VAR];
  bdd_init(10000000, 1000000);
  bdd_setcacheratio(10000);
  bdd_setvarnum(N_VAR);

  bdd p[M][N][N];

  init(p);

  bdd task = bddtrue;

  //additional limitation
  task &= limitation_1(p, 0, 4, 1);
  task &= limitation_1(p, 0, 5, 4);
  task &= limitation_1(p, 2, 6, 2);
  task &= limitation_1(p, 1, 7, 8);

  task &= limitation_1(p, 3, 2, 5);

  task &= limitation_1(p, 1, 4, 0);
  task &= limitation_1(p, 3, 4, 2);

  task &= limitation_1(p, 0, 6, 7);
  task &= limitation_1(p, 3, 6, 3);

  task &= limitation_1(p, 3, 8, 6);

  task &= limitation_2(p, 0, 0, 3, 7);

  task &= limitation_3(p, Neighbours::DOWN, 0, 1, 0, 5);
  task &= limitation_3(p, Neighbours::RIGHT_DOWN, 0, 1, 0, 6);

  //main limitation
  task &= limitation_1(p, 0, 0, 0);
  task &= limitation_1(p, 2, 0, 0);
  task &= limitation_1(p, 0, 1, 2);
  task &= limitation_1(p, 1, 2, 7);
  task &= limitation_1(p, 3, 3, 4);
  task &= limitation_1(p, 2, 3, 8);
  task &= limitation_1(p, 1, 3, 1);

  task &= limitation_2(p, 0, 0, 1, 5);
  task &= limitation_2(p, 0, 6, 2, 4);
  task &= limitation_2(p, 2, 1, 0, 2);
  task &= limitation_2(p, 3, 1, 1, 8);

  task &= limitation_3(p, Neighbours::RIGHT_DOWN, 2, 0, 0, 1);
  task &= limitation_3(p, Neighbours::DOWN, 1, 3, 2, 7);
  task &= limitation_3(p, Neighbours::DOWN, 3, 4, 1, 2);
  task &= limitation_3(p, Neighbours::RIGHT_DOWN, 0, 6, 0, 0);

  task &= limitation_4(p, 0, 8, 3, 8);
  task &= limitation_4(p, 0, 7, 0, 2);
  task &= limitation_4(p, 3, 1, 2, 1);
  task &= limitation_4(p, 0, 2, 2, 3);
  task &= limitation_4(p, 1, 5, 3, 4);

  task &= limitation_6(p);
  limitation_5(task, p);

  int satcount = bdd_satcount(task);
  std::cout << satcount << " solutions:\n" << std::endl;
  if (satcount)
  {
    bdd_allsat(task, fun);
  }
  std::cout << satcount << " solutions!\n" << std::endl;

  bdd_done();
  delete[] var;
  return 0;
}

void init(bdd p[M][N][N])
{
  for (int i = 0; i < M; ++i)
  {
    for (int j = 0; j < N; ++j)
    {
      for (int k = 0; k < N; ++k)
      {
        p[i][j][k] = bddtrue;
        for (int l = 0; l < LOG_N; ++l)
        {
          p[i][j][k] &= ((k >> l) & 1) ? bdd_ithvar((j * LOG_N * M) + l + LOG_N * i) : bdd_nithvar(
            (j * LOG_N * M) + l + LOG_N * i);
        }
      }
    }
  }
}

bdd limitation_1(bdd p[M][N][N], int propertyNumber, int objectNumber, int value)
{
  return p[propertyNumber][objectNumber][value];
}

bdd limitation_2(bdd p[M][N][N], int number1, int value1, int number2, int value2)
{
  bdd tree = bddtrue;

  for (int i = 0; i < N; ++i)
  {
    tree &= !(p[number1][i][value1] ^ p[number2][i][value2]);
  }
  return tree;
}

bdd limitation_3(bdd p[M][N][N], Neighbours neighbour, int number1, int value1, int number2, int value2)
{
  bdd tree = bddtrue;

  switch (neighbour)
  {
  case DOWN:
  {
    for (int i = 0; i < N; ++i)
    {
      if ((i <= N - 1 - N_SQRT))
      {
        tree &= !(p[number1][i][value1] ^ p[number2][i + N_SQRT][value2]);
      }
      else
      {
        tree &= !(p[number1][i][value1] ^ p[number2][std::abs(N - i - N_SQRT)][value2]);
      }
    }
    break;
  }
  case RIGHT_DOWN:
  {
    for (int i = 0; i < N; ++i)
    {
      if (i % N_SQRT == N_SQRT - 1 && i != N - 1)
      {
        tree &= !p[number1][i][value1];
      }
      else if (i == N - 1)
      {
        tree &= !(p[number1][i][value1] ^ p[number2][0][value2]);
      }
      else if (i >= N - N_SQRT)
      {
        tree &= !(p[number1][i][value1] ^ p[number2][N_SQRT - (N - i) + 1][value2]);
      }
      else
      {
        tree &= !(p[number1][i][value1] ^ p[number2][i + N_SQRT + 1][value2]);
      }
    }
    break;
  }
  default:
    assert("error");
    std::cerr << "Wrong enum";
    break;
  }
  return tree;
}

bdd limitation_4(bdd p[M][N][N], int number1, int value1, int number2, int value2)
{
  bdd tree = bddfalse;

  tree |= limitation_3(p, Neighbours::DOWN, number1, value1, number2, value2);
  tree |= limitation_3(p, Neighbours::RIGHT_DOWN, number1, value1, number2, value2);

  return tree;
}

void limitation_5(bdd& tree, bdd p[M][N][N])
{
  for (int m = 0; m < M; ++m)
  {
    for (int i = 0; i < N - 1; ++i)
    {
      for (int j = i + 1; j < N; ++j)
      {
        for (int k = 0; k < N; ++k)
        {
          tree &= p[m][i][k] >> !p[m][j][k];
        }
      }
    }
  }
}

bdd limitation_6(bdd p[M][N][N])
{
  bdd tree = bddtrue;

  for (int i = 0; i < N; ++i)
  {
    for (int k = 0; k < M; ++k)
    {
      bdd temp = bddfalse;
      for (int j = 0; j < N; ++j)
      {
        temp |= p[k][i][j];
      }

      tree &= temp;
    }
  }

  return tree;
}

void print()
{
  for (int i = 0; i < N; i++)
  {
    std::cout << i << ": ";
    for (int j = 0; j < M; j++)
    {
      int J = i * M * LOG_N + j * LOG_N;
      int num = 0;
      for (int k = 0; k < LOG_N; k++) num += var[J + k] << k;
      std::cout << num << ' ';
    }
    std::cout << "\n";
  }
  std::cout << "\n";
}

void build(char* varset, unsigned n, unsigned I)
{
  if (I == n - 1)
  {
    if (varset[I] >= 0)
    {
      var[I] = varset[I];
      print();
      return;
    }

    var[I] = 0;
    print();
    var[I] = 1;
    print();
    return;
  }

  if (varset[I] >= 0)
  {
    var[I] = varset[I];
    build(varset, n, I + 1);
    return;
  }

  var[I] = 0;
  build(varset, n, I + 1);
  var[I] = 1;
  build(varset, n, I + 1);
}

void fun(char* varset, int size)
{
  build(varset, size, 0);
}
