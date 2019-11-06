#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <sstream>

using namespace std;

// void printNice(string* firstCol, double** matrix, double** matrixErr, string* header, int nrRows, int nrCols, int nSigFig, ostream& ost, string delim, string ending);
// void printNice(double** matrix, double** matrixErr, string* header, int nrRows, int nrCols, int nSigFig, string delim, string ending);
// void printNice(double** matrix, double** matrixErr, string* header, int nrRows, int nrCols, int nSigFig, ostream& ost, ostream& ost, string delim, string ending);

void printNice(double** matrix, double** matrixErr, string* header, int nrRows, int nrCols, int nSigFig, ostream& ost, string delim = " & ", string ending = " \\\\")
{
  int** lmatrix = new int*[nrRows];
  int** lmatrix_pm = new int*[nrRows];
  for (int i=0;i<nrRows;i++)
  {
    lmatrix[i] = new int[nrCols];
    lmatrix_pm[i] = new int[nrCols];
  }

  string pm = " $\\pm$ ";
  // int lpm = pm.size();

  int* maxLength = new int[nrCols];
  int* maxLength_pm = new int[nrCols];
  for (int j=0;j<nrCols;j++) 
  {
    if (header) maxLength[j] = header[j].size();
    else maxLength[j] = 0;
    maxLength_pm[j] = 0;
  }
  for (int i=0;i<nrRows;i++)
  {
    for (int j=0;j<nrCols;j++)
    {
      stringstream str;
      str << setprecision(nSigFig);
      str << matrix[i][j];

      lmatrix[i][j] = str.str().size();
      maxLength[j] = int(max(double(maxLength[j]), double(lmatrix[i][j])));

      stringstream str2;
      str2 << setprecision(nSigFig);
      if (j != 0 && matrixErr) str2 << pm << matrixErr[i][j];

      lmatrix_pm[i][j] = str2.str().size();
      maxLength_pm[j] = int(max(double(maxLength_pm[j]), double(lmatrix_pm[i][j])));

      // ost << "lmatrix " << i << "," << j << ": " << lmatrix[i][j] << endl;
      // ost << "maxLength " << j << ": " << maxLength[j] << endl;
    }
  }

  if (header)
  {
    for (int i=0;i<nrCols;i++)
    {
      ost << header[i];
   
      for (int k=header[i].size();k<maxLength[i]+maxLength_pm[i];k++)
      {
        ost << " ";
      }
      if (i < nrCols-1)
      {
        if (delim != "") ost << delim;
      }
      else 
      {
        if (ending != "") ost << ending;
        ost << "\n";
      }
    }
  }

  for (int i=0;i<nrRows;i++)
  {
    for (int j=0;j<nrCols;j++)
    {
      ost << setprecision(nSigFig);
      ost << matrix[i][j];

      for (int k=lmatrix[i][j];k<maxLength[j];k++)
      {
        ost << " ";
      }

      if (!header) ost << " ";

      if (j != 0 && matrixErr) 
      {
         ost << pm << matrixErr[i][j];
        for (int k=lmatrix_pm[i][j];k<maxLength_pm[j];k++)
        {
          ost << " ";
        }
      }

      if (j < nrCols-1)
      {
        if (delim != "") ost << delim;
      }
      else 
      {
        if (ending != "") ost << ending;
        ost << "\n";
      }
    }
  }
  delete lmatrix;
  delete lmatrix_pm;
}

void printNice(string* firstCol, double** matrix, double** matrixErr, string* header, int nrRows, int nrCols, int nSigFig, ostream& ost, string delim = " & ", string ending = " \\\\")
{
  int** lmatrix = new int*[nrRows];
  int** lmatrix_pm = new int*[nrRows];
  for (int i=0;i<nrRows;i++)
  {
    lmatrix[i] = new int[nrCols];
    lmatrix_pm[i] = new int[nrCols];
  }

  int maxLfirst = 0;
  for (int i=0;i<nrRows+1;i++) 
  {
    maxLfirst = int(max(double(maxLfirst), double(firstCol[i].size())));
  }

  string pm = " $\\pm$ ";
  // int lpm = pm.size();

  int* maxLength = new int[nrCols];
  int* maxLength_pm = new int[nrCols];
  for (int j=0;j<nrCols;j++) 
  {
    if (header) maxLength[j] = header[j].size();
    else maxLength[j] = 0;
    maxLength_pm[j] = 0;
    // cout << "col: " << j << " / " << nrCols << endl;
  }
  for (int i=0;i<nrRows;i++)
  {
    for (int j=0;j<nrCols;j++)
    {
      stringstream str;
      str << setprecision(nSigFig);
      str << matrix[i][j];

      lmatrix[i][j] = str.str().size();
      maxLength[j] = int(max(double(maxLength[j]), double(lmatrix[i][j])));

      stringstream str2;
      str2 << setprecision(nSigFig);
      if (/*j != 0 && */matrixErr) str2 << pm << matrixErr[i][j];

      lmatrix_pm[i][j] = str2.str().size();
      maxLength_pm[j] = int(max(double(maxLength_pm[j]), double(lmatrix_pm[i][j])));

      // cout << "lmatrix " << i << "," << j << ": " << lmatrix[i][j] << endl;
      // cout << "maxLength " << j << ": " << maxLength[j] << endl;
    }
  }
  if (header)
  {
    ost << firstCol[0];
    for (int j=firstCol[0].size();j<maxLfirst;j++) ost << " ";
    ost << " & ";
    for (int i=0;i<nrCols;i++)
    {
      ost << header[i];
   
      for (int k=header[i].size();k<maxLength[i]+maxLength_pm[i];k++)
      {
        ost << " ";
      }
      if (i < nrCols-1)
      {
        if (delim != "") ost << delim;
      }
      else 
      {
        if (ending != "") ost << ending;
        ost << "\n";
      }
    }
  }

  for (int i=0;i<nrRows;i++)
  {
    ost << firstCol[i+1];
    for (int j=firstCol[i+1].size();j<maxLfirst;j++) ost << " ";
    ost << " & ";
    for (int j=0;j<nrCols;j++)
    {
      ost << setprecision(nSigFig);
      ost << matrix[i][j];

      for (int k=lmatrix[i][j];k<maxLength[j];k++)
      {
        ost << " ";
      }

      if (/*j != 0 && */matrixErr) 
      {
        ost << pm << matrixErr[i][j];
        for (int k=lmatrix_pm[i][j];k<maxLength_pm[j];k++)
        { 
          ost << " ";
        }
      }

      if (j < nrCols-1)
      {
        if (delim != "") ost << delim;
      }
      else 
      {
        if (ending != "") ost << ending;
        ost << "\n";
      }
    }
  }

  delete lmatrix;
  delete lmatrix_pm;
}

void printNice(double** matrix, double** matrixErr, string* header, int nrRows, int nrCols, int nSigFig, string delim = " & ", string ending = " \\\\")
{
  printNice(matrix, matrixErr, header, nrRows, nrCols, nSigFig, cout, delim, ending);
}
