
using System;
using System.Collections.Generic;
using System.Text;
using Excel;
using System.Diagnostics;
using System.Reflection;

using Microsoft.Office.Interop.Excel;

using Twitter_twitinvi;

namespace MyClass.WriteToExcel
{
    public abstract class ExcelFileWriter<T>
    {
        private Microsoft.Office.Interop.Excel.Application _excelApplication = null;
        private Microsoft.Office.Interop.Excel.Workbooks _workBooks = null;
        private Microsoft.Office.Interop.Excel._Workbook _workBook = null;
        private object _value = Missing.Value;
        private Microsoft.Office.Interop.Excel.Sheets _excelSheets = null;
        private Microsoft.Office.Interop.Excel._Worksheet _excelSheet = null;
        private Microsoft.Office.Interop.Excel.Range _excelRange = null;
        private Microsoft.Office.Interop.Excel.Font _excelFont = null;
        /// <summary>
        /// User have to set the names of header in the derived class
        /// </summary>
        public abstract object[] Headers { get;}
        /// <summary>
        /// user have to parse the data from the list and pass each data along with the
        /// column and row name to the base fun, FillExcelWithData().
        /// </summary>
        /// <param name="list"></param>
        public abstract void FillRowData(List<Hook> list);

        /// <summary>
        /// get the data of object which will be saved to the excel sheet
        /// </summary>
        public abstract object[,] ExcelData { get;}
        /// <summary>
        /// get the no of columns
        /// </summary>
        public abstract int ColumnCount { get;}
        /// <summary>
        /// get the now of rows to fill
        /// </summary>
        public abstract int RowCount { get;}

        /// <summary>
        /// user can override this to make the headers not be in bold.
        /// by default it is true
        /// </summary>
        protected virtual bool BoldHeaders
        {
            get
            {
                return true;
            }
        }

        /// <summary>
        /// api through which data from the list can be write to an excel
        /// kind of a Template Method Pattern is used here
        /// </summary>
        /// <param name="fileName"></param>
        /// <param name="holdingsList"></param>
        public void WriteDateToExcel(string fileName, List<Hook> list, string startColumn, string endColumn)
        {
            this.ActivateExcel();                        
            this.FillRowData(list);
            this.FillExcelWithData();
            this.FillHeaderColumn(Headers, startColumn, endColumn);
            this.SaveExcel(fileName);
        }
        
        /// <summary>
        /// activate the excel application
        /// </summary>
        protected virtual void ActivateExcel()
        {
            _excelApplication = new Microsoft.Office.Interop.Excel.Application();
            _workBooks = (Microsoft.Office.Interop.Excel.Workbooks)_excelApplication.Workbooks;
            _workBook = (Microsoft.Office.Interop.Excel._Workbook)(_workBooks.Add(_value));
            _excelSheets = (Microsoft.Office.Interop.Excel.Sheets)_workBook.Worksheets;
            _excelSheet = (Microsoft.Office.Interop.Excel._Worksheet)(_excelSheets.get_Item(1)); 
        }

        /// <summary>
        /// fill the header columns for the range specified and make it bold if specified
        /// </summary>
        /// <param name="headers"></param>
        /// <param name="startColumn"></param>
        /// <param name="endColumn"></param>
        protected void FillHeaderColumn(object[] headers, string startColumn, string endColumn)
        {                       
            _excelRange = _excelSheet.get_Range(startColumn, endColumn);
            _excelRange.set_Value(_value, headers);
            if (BoldHeaders == true)
            {
                this.BoldRow(startColumn, endColumn);
            }
            _excelRange.EntireColumn.AutoFit();
        }
        /// <summary>
        /// Fill the excel sheet with data along with the position specified
        /// </summary>
        /// <param name="columnrow"></param>
        /// <param name="data"></param>
        private void FillExcelWithData()
        {
            _excelRange = _excelSheet.get_Range("A1", _value);
            _excelRange = _excelRange.get_Resize(RowCount + 1, ColumnCount);
            _excelRange.set_Value(Missing.Value, ExcelData);
            _excelRange.EntireColumn.AutoFit();
        }
        /// <summary>
        /// save the excel sheet to the location with file name
        /// </summary>
        /// <param name="fileName"></param>
        protected virtual void SaveExcel(string fileName)
        {
            _workBook.SaveAs(fileName, _value, _value,
                _value, _value, _value, Microsoft.Office.Interop.Excel.XlSaveAsAccessMode.xlNoChange,
                _value, _value, _value, _value, null);
            _workBook.Close(false, _value, _value);
            _excelApplication.Quit();
        }
        /// <summary>
        /// make the range of rows bold
        /// </summary>
        /// <param name="row1"></param>
        /// <param name="row2"></param>
        private void BoldRow(string row1, string row2)
        {            
            _excelRange = _excelSheet.get_Range(row1, row2);
            _excelFont = _excelRange.Font;
            _excelFont.Bold = true;
        }
    }



    public class ExcelWrite : ExcelFileWriter<Hook>
    {
        public object[,] myExcelData;
        private int myRowCnt;
        public override object[] Headers
        {
            get
            {
                object[] headerName = { "Hook", "Category", "Weather", "Timeofday" };
                return headerName;
            }
        }

        public override void FillRowData(List<Hook> list)
        {

            myRowCnt = list.Count;
            myExcelData = new object[RowCount + 1, 4];
            for (int row = 1; row <= myRowCnt; row++)
            {
                for (int col = 0; col < 4; col++)
                {
                    myExcelData[row, col] = list[row - 1];
                }
            }
        }

        public override object[,] ExcelData
        {
            get
            {
                return myExcelData;
            }
        }

        public override int ColumnCount
        {
            get
            {
                return 4;
            }
        }

        public override int RowCount
        {
            get
            {
                return myRowCnt;
            }
        }
    }

  }
