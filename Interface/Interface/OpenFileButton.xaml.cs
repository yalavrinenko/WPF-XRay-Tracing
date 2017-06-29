using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Microsoft.Win32;

namespace Interface
{
    /// <summary>
    /// Логика взаимодействия для OpenFileButton.xaml
    /// </summary>
    public partial class OpenFileButton : UserControl
    {
        public String SelectedFileName { get; set; }
        public bool Result { get; set; }
        public string Extension { get; set; }

        public string ButtonLabelContent { get { return (SelectedFileName!=null && SelectedFileName!="") ? System.IO.Path.GetFileName(SelectedFileName) : "Open File"; } }
        public class FileSelectionArgs
        {
            public bool isSelected;
            public string FileName;
        }

        public event EventHandler<FileSelectionArgs> FileSelect = null;

        public OpenFileButton()
        {
            InitializeComponent();

            SelectedFileName = "";
            Result = false;
            Extension = "All files (*.*)|*.*";
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog m_openDialog = new OpenFileDialog();
            m_openDialog.Filter = this.Extension;
            if (m_openDialog.ShowDialog() == true)
            {
                Result = true;
                SelectedFileName = m_openDialog.FileName;
                this.ButtonLabel.Content = ButtonLabelContent;
            }
            else
            {
                Result = false;
                SelectedFileName = "";
            }

            FileSelect?.Invoke(this, new FileSelectionArgs() { isSelected = Result, FileName = SelectedFileName });
        }

        private void ButtonLabel_Loaded(object sender, RoutedEventArgs e)
        {
            this.ButtonLabel.Content = ButtonLabelContent;
        }
    }
}
