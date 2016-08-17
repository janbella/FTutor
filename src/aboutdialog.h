#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

/**
 * @file aboutdialog.h
 * @author Ján Bella
 */

#include "glob_includes.h"
#include "localization.h"

/**
 * @brief The AboutDialog class is a simple dialog window with information about the application and its creator
 */
class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief AboutDialog costructor
     * @param parent the parent object, should be MainWindow
     * @param localization the instance of Localization class, which provides translated labels
     */
    explicit AboutDialog(QWidget *parent, const Translation* language, QString icon);

    /**
     * Desctructor
     */
    virtual ~AboutDialog();

private:

    // the following are QtWidgets in the window
    QGraphicsView *logoGraphicsView;
    QFrame *line;
    QFrame *line2;
    QLabel *labelAppName;
    QLabel *labelVersion;
    QLabel *labelLongName;
    QLabel *labelAuthor;
    QLabel *labelAuthorName;
    QLabel *labelCredits;
    QLabel *labelInstitution;
    QLabel *labelYear;

    /**
     * @brief setDefaultTexts sets defaults values to each text or title or label in the window.
     */
    void setDefaultTexts();

    /**
     * @brief setLocalizedTexts sets text, title or label values according to given Translation object`
     * @param language Translation object used to set texts
     */
    void setLocalizedTexts(const Translation* language);
};

#endif // ABOUTDIALOG_H
