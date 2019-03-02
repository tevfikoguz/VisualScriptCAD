#ifndef TRANSFORMSHAPENODES_HPP
#define TRANSFORMSHAPENODES_HPP

#include "ShapeNode.hpp"

class TranslateShapeNode : public ShapeNode
{
	DYNAMIC_SERIALIZABLE (TranslateShapeNode);

public:
	TranslateShapeNode ();
	TranslateShapeNode (const std::wstring& name, const NUIE::Point& position);

	virtual void				Initialize () override;
	virtual NE::ValueConstPtr	Calculate (NE::EvaluationEnv& env) const override;

	virtual NE::Stream::Status	Read (NE::InputStream& inputStream) override;
	virtual NE::Stream::Status	Write (NE::OutputStream& outputStream) const override;
};

class RotateShapeNode : public ShapeNode
{
	DYNAMIC_SERIALIZABLE (RotateShapeNode);

public:
	RotateShapeNode ();
	RotateShapeNode (const std::wstring& name, const NUIE::Point& position);

	virtual void				Initialize () override;
	virtual void				RegisterParameters (NUIE::NodeParameterList& parameterList) const;
	virtual NE::ValueConstPtr	Calculate (NE::EvaluationEnv& env) const override;

	virtual NE::Stream::Status	Read (NE::InputStream& inputStream) override;
	virtual NE::Stream::Status	Write (NE::OutputStream& outputStream) const override;
};

class TransformShapeNode : public ShapeNode
{
	DYNAMIC_SERIALIZABLE (TransformShapeNode);

public:
	TransformShapeNode ();
	TransformShapeNode (const std::wstring& name, const NUIE::Point& position);

	virtual void				Initialize () override;
	virtual NE::ValueConstPtr	Calculate (NE::EvaluationEnv& env) const override;

	virtual NE::Stream::Status	Read (NE::InputStream& inputStream) override;
	virtual NE::Stream::Status	Write (NE::OutputStream& outputStream) const override;
};

#endif
