#ifndef SHAPENODES_HPP
#define SHAPENODES_HPP

#include "Model.hpp"
#include "Shape.hpp"
#include "ShapeNode.hpp"
#include "Basic3DNodeValues.hpp"

class BoxNode : public ShapeNode
{
	DYNAMIC_SERIALIZABLE (BoxNode);

public:
	BoxNode ();
	BoxNode (const std::wstring& name, const NUIE::Point& position);

	virtual void				Initialize () override;
	virtual NE::ValueConstPtr	Calculate (NE::EvaluationEnv& env) const override;
	virtual void				RegisterParameters (NUIE::NodeParameterList& parameterList) const;

	virtual NE::Stream::Status	Read (NE::InputStream& inputStream) override;
	virtual NE::Stream::Status	Write (NE::OutputStream& outputStream) const override;
};

class BoxShellNode : public ShapeNode
{
	DYNAMIC_SERIALIZABLE (BoxShellNode);

public:
	BoxShellNode ();
	BoxShellNode (const std::wstring& name, const NUIE::Point& position);

	virtual void				Initialize () override;
	virtual NE::ValueConstPtr	Calculate (NE::EvaluationEnv& env) const override;
	virtual void				RegisterParameters (NUIE::NodeParameterList& parameterList) const;

	virtual NE::Stream::Status	Read (NE::InputStream& inputStream) override;
	virtual NE::Stream::Status	Write (NE::OutputStream& outputStream) const override;
};

class CylinderNode : public ShapeNode
{
	DYNAMIC_SERIALIZABLE (CylinderNode);

public:
	CylinderNode ();
	CylinderNode (const std::wstring& name, const NUIE::Point& position);

	virtual void				Initialize () override;
	virtual NE::ValueConstPtr	Calculate (NE::EvaluationEnv& env) const override;
	virtual void				RegisterParameters (NUIE::NodeParameterList& parameterList) const;

	virtual NE::Stream::Status	Read (NE::InputStream& inputStream) override;
	virtual NE::Stream::Status	Write (NE::OutputStream& outputStream) const override;
};

class CylinderShellNode : public ShapeNode
{
	DYNAMIC_SERIALIZABLE (CylinderShellNode);

public:
	CylinderShellNode ();
	CylinderShellNode (const std::wstring& name, const NUIE::Point& position);

	virtual void				Initialize () override;
	virtual NE::ValueConstPtr	Calculate (NE::EvaluationEnv& env) const override;
	virtual void				RegisterParameters (NUIE::NodeParameterList& parameterList) const;

	virtual NE::Stream::Status	Read (NE::InputStream& inputStream) override;
	virtual NE::Stream::Status	Write (NE::OutputStream& outputStream) const override;
};

class ConeNode : public ShapeNode
{
	DYNAMIC_SERIALIZABLE (ConeNode);

public:
	ConeNode ();
	ConeNode (const std::wstring& name, const NUIE::Point& position);

	virtual void				Initialize () override;
	virtual NE::ValueConstPtr	Calculate (NE::EvaluationEnv& env) const override;
	virtual void				RegisterParameters (NUIE::NodeParameterList& parameterList) const;

	virtual NE::Stream::Status	Read (NE::InputStream& inputStream) override;
	virtual NE::Stream::Status	Write (NE::OutputStream& outputStream) const override;
};

class SphereNode : public ShapeNode
{
	DYNAMIC_SERIALIZABLE (SphereNode);

public:
	SphereNode ();
	SphereNode (const std::wstring& name, const NUIE::Point& position);

	virtual void				Initialize () override;
	virtual NE::ValueConstPtr	Calculate (NE::EvaluationEnv& env) const override;
	virtual void				RegisterParameters (NUIE::NodeParameterList& parameterList) const;

	virtual NE::Stream::Status	Read (NE::InputStream& inputStream) override;
	virtual NE::Stream::Status	Write (NE::OutputStream& outputStream) const override;
};

class TorusNode : public ShapeNode
{
	DYNAMIC_SERIALIZABLE (TorusNode);

public:
	TorusNode ();
	TorusNode (const std::wstring& name, const NUIE::Point& position);

	virtual void				Initialize () override;
	virtual NE::ValueConstPtr	Calculate (NE::EvaluationEnv& env) const override;
	virtual void				RegisterParameters (NUIE::NodeParameterList& parameterList) const;

	virtual NE::Stream::Status	Read (NE::InputStream& inputStream) override;
	virtual NE::Stream::Status	Write (NE::OutputStream& outputStream) const override;
};

class PlatonicNode : public ShapeNode
{
	DYNAMIC_SERIALIZABLE (PlatonicNode);

public:
	PlatonicNode ();
	PlatonicNode (const std::wstring& name, const NUIE::Point& position);

	virtual void				Initialize () override;
	virtual NE::ValueConstPtr	Calculate (NE::EvaluationEnv& env) const override;
	virtual void				RegisterCommands (NUIE::NodeCommandRegistrator& commandRegistrator) const;
	virtual void				RegisterParameters (NUIE::NodeParameterList& parameterList) const;

	virtual NE::Stream::Status	Read (NE::InputStream& inputStream) override;
	virtual NE::Stream::Status	Write (NE::OutputStream& outputStream) const override;

	Modeler::PlatonicSolidType	GetType () const;
	void						SetType (Modeler::PlatonicSolidType newType);

private:
	Modeler::PlatonicSolidType	type;
};

#endif
