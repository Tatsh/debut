#ifndef PREFSDIALOG_H
#define PREFSDIALOG_H

#include "ui_prefsdialog.h"

class PrefsDialog : public QDialog, private Ui::PrefsDialog {
  Q_OBJECT
      
  public:
    PrefsDialog(QDialog *parent = 0);
    
  private slots:
};

#endif
