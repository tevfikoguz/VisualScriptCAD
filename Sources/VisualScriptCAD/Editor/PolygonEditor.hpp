#ifndef POLYGONEDITOR_HPP
#define POLYGONEDITOR_HPP

#include "IncludeGLM.hpp"
#include <wx/wx.h>

class PolygonEditor
{
public:
	PolygonEditor (const std::vector<glm::dvec2>& polygon);

	void							UpdateScreenSize (const wxSize& newScreenSize);
	void							UpdateMousePosition (const wxPoint& point);
	void							UpdateScale (int change);

	glm::dvec2						GetMousePositionAsPolygonPoint () const;
	void							HandleMouseClick (const wxPoint& point);

	bool							HasPolygon () const;
	const std::vector<glm::dvec2>&	GetPolygon () const;

	bool							HasSelectedVertex () const;
	int								GetSelectedVertex () const;

	glm::dvec2						GetSelectedVertexPosition () const;
	void							SetSelectedVertexPosition (const glm::dvec2 position);

	int								GetScale () const;
	const wxPoint&					GetMouseScreenPosition () const;

	glm::dvec2						MouseCoordToPolygonPoint (const wxPoint& point) const;
	wxPoint							PolygonPointToMouseCoord (const glm::dvec2& point) const;

private:
	wxPoint							MouseCoordToCenteredCoord (const wxPoint& point) const;
	wxPoint							CenteredCoordToMouseCoord (const wxPoint& point) const;
	int								DetectVertexUnderMouse (const wxPoint& point) const;

	std::vector<glm::dvec2>			polygon;
	bool							closed;

	wxSize							screenSize;
	wxPoint							mouseScreenPosition;

	int								selectedVertex;
	int								scale;
};

class PolygonEditorPanel : public wxPanel
{
public:
	class StatusUpdater
	{
	public:
		virtual ~StatusUpdater ();

		virtual void UpdateStatus (int scale, const glm::dvec2& position) = 0;
	};

	PolygonEditorPanel (wxWindow* parent, const std::vector<glm::dvec2>& polygon, StatusUpdater* statusUpdater);

	void							OnPaint (wxPaintEvent& evt);
	void							OnResize (wxSizeEvent& evt);

	void							OnLeftClick (wxMouseEvent& evt);
	void							OnRightClick (wxMouseEvent& evt);
	void							OnMouseMove (wxMouseEvent& evt);
	void							OnMouseWheel (wxMouseEvent& evt);

	bool							HasPolygon () const;
	std::vector<glm::dvec2>			GetPolygon () const;

	void							UpdateStatus ();

private:
	void							Draw ();
	void							DrawCoordSystem (wxDC& dc);
	void							DrawPolygon (wxDC& dc);

	StatusUpdater*					statusUpdater;
	PolygonEditor					polygonEditor;
	wxBitmap						memoryBitmap;
	wxMemoryDC						memoryDC;

	DECLARE_EVENT_TABLE ();
};

class PolygonEditorDialog : public wxDialog
{
public:
	class StatusUpdater : public PolygonEditorPanel::StatusUpdater
	{
	public:
		StatusUpdater (PolygonEditorDialog* dialog);

		virtual void UpdateStatus (int scale, const glm::dvec2& position) override;

	private:
		PolygonEditorDialog* dialog;
	};

	PolygonEditorDialog (wxWindow *parent, const std::vector<glm::dvec2>& polygon);

	void						OnButtonClick (wxCommandEvent& evt);
	void						OnClose (wxCloseEvent& evt);
	
	bool						HasPolygon () const;
	std::vector<glm::dvec2>		GetPolygon () const;

private:
	StatusUpdater			statusUpdater;
	wxBoxSizer*				boxSizer;
	PolygonEditorPanel*		editorPanel;
	wxButton*				okButton;
	wxStatusBar*			statusBar;

	DECLARE_EVENT_TABLE ();
};

#endif