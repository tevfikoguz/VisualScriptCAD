#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "ModelEvaluationData.hpp"
#include "NodeEditorControl.hpp"
#include "ModelControl.hpp"

#include <wx/wx.h>
#include <wx/splitter.h>

enum CommandId
{
	File_New					= 1,
	File_Open					= 2,
	File_Save					= 3,
	File_SaveAs					= 4,
	File_Exit					= 5,
	Edit_Undo					= 6,
	Edit_Redo					= 7,
	Mode_Automatic				= 8,
	Mode_Manual					= 9,
	Mode_Update					= 10,
	View_Editor					= 11,
	View_Model					= 12,
	View_Split					= 13,
	Editor_FitToWindow			= 14,
	Model_FitToWindow			= 15,
	Model_ViewMode_Lines		= 16,
	Model_ViewMode_Polygons		= 17,
	Model_Info					= 18,
	Model_Export				= 19,
	About_GitHub				= 20
};

enum class ViewMode
{
	Editor,
	Model,
	Split
};

class EvaluationData : public ModelEvaluationData
{
public:
	EvaluationData (Modeler::Model& model);
	virtual ~EvaluationData ();

	virtual Modeler::Model&						GetModel () override;
	virtual Modeler::MeshId						AddMesh (const Modeler::Mesh& mesh) override;
	virtual void								RemoveMesh (Modeler::MeshId meshId) override;

	const Modeler::Model&						GetModel () const;

	const std::unordered_set<Modeler::MeshId>&	GetAddedMeshes () const;
	const std::unordered_set<Modeler::MeshId>&	GetDeletedMeshes () const;
	void										Clear ();

private:
	Modeler::Model&							model;
	std::unordered_set<Modeler::MeshId>		addedMeshes;
	std::unordered_set<Modeler::MeshId>		deletedMeshes;
};

class ModelControlSynchronizer : public ModelSynchronizer
{
public:
	ModelControlSynchronizer (std::shared_ptr<EvaluationData>& evalData, ModelControl* modelControl);

	virtual void Synchronize () override;

private:
	std::shared_ptr<EvaluationData>& evalData;
	ModelControl* modelControl;
};

class MenuBar : public wxMenuBar
{
public:
	MenuBar ();

	void	UpdateStatus (ViewMode viewMode, WXAS::NodeEditorControl::UpdateMode updateMode);
};

class ToolBar : public wxToolBar
{
public:
	ToolBar (wxWindow* parent);

	void	UpdateStatus (ViewMode viewMode, WXAS::NodeEditorControl::UpdateMode updateMode);

private:
	wxToolBarToolBase*	AddIconButton (int toolId, const void* imageData, size_t imageSize, std::wstring helpText);
	wxToolBarToolBase*	AddRadioButton (int toolId, const void* imageData, size_t imageSize, std::wstring helpText);
};

class ApplicationState
{
public:
	ApplicationState ();

	void					ClearCurrentFileName ();
	void					SetCurrentFileName (const std::wstring& newCurrentFileName);
	bool					HasCurrentFileName () const;
	const std::wstring&		GetCurrentFileName () const;

private:
	std::wstring currentFileName;
};

class MainWindow : public wxFrame
{
public:
	MainWindow (const std::wstring& fileName);
	~MainWindow ();

	void	OnCommand (wxCommandEvent& evt);
	void	OnClose (wxCloseEvent& event);
	void	ProcessCommand (CommandId commandId);
	void	OnKeyDown (wxKeyEvent& evt);

private:
	bool	ConfirmLosingUnsavedChanges ();
	void	UpdateMenuBar ();
	void	UpdateStatusBar ();

	void	NewFile ();
	void	OpenFile (const std::wstring& fileName);

	Modeler::Model						model;
	std::shared_ptr<EvaluationData>		evaluationData;
	MenuBar*							menuBar;
	ToolBar*							toolBar;
	wxSplitterWindow*					editorAndModelSplitter;
	ModelControl*						modelControl;
	ModelControlSynchronizer			modelControlSynchronizer;
	NodeEditorControl*					nodeEditorControl;

	ApplicationState					applicationState;
	ViewMode							viewMode;
	int									sashPosition;

	DECLARE_EVENT_TABLE ()
};

#endif