#ifndef APPEARANCE_H
#define APPEARANCE_H

#include <QTableView>
#include <QHeaderView>

static void SetTableAppearance(QTableView *table_view) {
    // This is actually songtablemodel appearance but whatever
    table_view->resizeColumnsToContents();
    // The actual table
    table_view->setShowGrid(false); // Hide the grid
    table_view->setSelectionBehavior(QAbstractItemView::SelectRows); // Select whole row
    //tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    table_view->setEditTriggers(QAbstractItemView::NoEditTriggers); // Disable editing
    table_view->verticalHeader()->setVisible(false); // Hide row numbers
    table_view->horizontalHeader()->setSectionsMovable(false); // Prevent column dragging
    table_view->horizontalHeader()->setHighlightSections(false); // Remove column click highlight

    // --- Column Widths & Alignment ---
    table_view->setColumnWidth(0, 40); // Track Number (Fixed width)
    table_view->setColumnWidth(3, 80); // Duration (Fixed width)
    //table_view->horizontalHeader()->setStretchLastSection(false);
    table_view->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch); // Song Name column expands
    table_view->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

    // --- Apply Theme ---
    table_view->setStyleSheet(
        "QTableView {"
        "   background-color: #121212;"
        "   color: white;" // White text
        "   selection-background-color: #333333;" // Dark Gray when row is selected
        "   gridline-color: #121212;"
        " 	outline: none;"
        "} "
        "QTableView::item {"
        "   background-color: #121212;" // Ensure all rows are uniform dark color
        "   padding: 5px;"
        "} "
        "QTableView::item:selected {"
        "   background-color: #333333;" // Row selection color
        "   color: white;"
        "} "
        "QTableView::item:focus {"
        "   outline: 0;"
        "	background: none;"
        "   border: none;" // Removes dashed border on click
        "}"
        "QTableView::item:!selected:focus {"
        "	background:transparent;"
        "}"
        "QTableView::item:selected:focus { background:#333333; }"
        "QHeaderView::section {		"
        "background-color: #121212;		"
        "color: lightgray;		"
        "font-weight: normal;		"
        "padding: 5px;		"
        "border: 1px solid #292929;	"
        "}	"
        "QHeaderView::section {"
        "border-left: none;"
        "border-right: none;"
        "}"
    );

}
#endif // APPEARANCE_H
