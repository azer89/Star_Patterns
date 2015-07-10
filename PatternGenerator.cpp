
#include <stdlib.h>
#include <iostream>
#include <algorithm>

#include "tinyxml2.h"

#include "ARay.h"
#include "VertexData.h"
#include "PatternGenerator.h"


PatternGenerator::PatternGenerator() /*: sideDiv(1)*/
{
}

PatternGenerator::~PatternGenerator()
{
}

void PatternGenerator::ReadXML(std::string filename)
{
    using namespace tinyxml2;

    XMLDocument doc;
    doc.LoadFile( filename.c_str() );
    std::cout << "XMl error: " << doc.ErrorID() << "\n\n";

    for( XMLNode* node = doc.FirstChildElement("tiling-library")->FirstChildElement("tiling");
        node;
        node = node->NextSibling() )
    {
        TilingData tilingData;

        XMLElement* elem = node->ToElement();
        tilingData._name = elem->Attribute("name");
        //std::string tiling_name = elem->Attribute("name");
        //std::cout << "(" << tiling_name << ")" << "\n";

        // translations
        XMLNode* tNode = node->FirstChildElement("translations");
        XMLNode* vNode1 = tNode->FirstChildElement("vector");
        XMLNode* vNode2 = vNode1->NextSibling();

        XMLElement* vElement1 = vNode1->ToElement();
        XMLElement* vElement2 = vNode2->ToElement();

        tilingData._translation1 = AVector(std::stof(vElement1->Attribute("x")), std::stof(vElement1->Attribute("y")));
        tilingData._translation2 = AVector(std::stof(vElement2->Attribute("x")), std::stof(vElement2->Attribute("y")));
        //std::cout << "(" << trans1.x << ", " << trans1.y << ") (" << trans2.x << ", " << trans2.y << ")\n";

        // tiles
        for( XMLNode* tileNode = tNode->NextSibling();
            tileNode;
            tileNode = tileNode->NextSibling() )
        {
            TileData tileData;

            // shape
            XMLNode* shapeNode = tileNode->FirstChildElement("shape");
            XMLElement* shapeElement = shapeNode->ToElement();

            std::string typeStr = shapeElement->Attribute("type");

            ShapeType sType;
            if(typeStr == "regular")
            {
                sType = ShapeType::S_REGULAR;
                tileData._sides = std::stof(shapeElement->Attribute("sides"));
            }
            else if(typeStr == "polygon")
            {
                sType = ShapeType::S_POLYGON;

                for( XMLNode* vertexNode = shapeNode->FirstChildElement("vertex");
                    vertexNode;
                    vertexNode = vertexNode->NextSibling() )
                {
                    XMLElement* vertexElement = vertexNode->ToElement();
                    tileData._vertices.push_back(AVector(std::stof(vertexElement->Attribute("x")),
                                                         std::stof(vertexElement->Attribute("y"))));
                    //std::cout << "v " << std::stof(vertexElement->Attribute("x")) << " " << std::stof(vertexElement->Attribute("y")) << "\n";
                }
                // to do: why reverse ?
                std::reverse(tileData._vertices.begin(), tileData._vertices.end());
            }

            tileData._shapeType = sType;


            //float sidesStr = std::stof(shapeElement->Attribute("sides"));
            //std::cout << typeStr << " " << sidesStr << "\n";

            // transformations
            for( XMLNode* transformNode = tileNode->FirstChildElement("transform");
                transformNode;
                transformNode = transformNode->NextSibling() )
            {
                QMatrix3x3 mat;

                //std::cout << "t";
                XMLElement* transformElement = transformNode->ToElement();

                mat(0, 0) = std::stof(transformElement->Attribute("a"));
                mat(0, 1) = std::stof(transformElement->Attribute("b"));
                mat(0, 2) = std::stof(transformElement->Attribute("c"));

                mat(1, 0) = std::stof(transformElement->Attribute("d"));
                mat(1, 1) = std::stof(transformElement->Attribute("e"));
                mat(1, 2) = std::stof(transformElement->Attribute("f"));

                tileData._transforms.push_back(mat);
                //std::cout << mat(0, 0) << " " << mat(0, 1) << " " << mat(0, 2) << " "
                //          << mat(1, 0) << " " << mat(1, 1) << " " << mat(1, 2) << "\n";
            }
            tilingData._tiles.push_back(tileData);
            //std::cout << "\n";
        }
        //std::cout << "\n";
        this->_tilings.push_back(tilingData);
    }
    //std::cout << _tilings.size() << "\n";

}

TilingData PatternGenerator::GetTiling(std::string tilingName)
{
    TilingData tilingData;
    for(int a = 0; a < _tilings.size(); a++)
    {
        if(_tilings[a]._name == tilingName)
        {
            tilingData = _tilings[a];
            break;
        }
    }

    return tilingData;
}

void PatternGenerator::InferenceAlgorithm(std::vector<std::vector<ALine>> shapes)
{
    _rayLines.clear();

    float eps_val = std::numeric_limits<float>::epsilon() * 1000.0f;

    float angle1 = -M_PI / 4.0f;
    float angle2 = -M_PI - angle1;

    float cos1 = cos(angle1);
    float sin1 = sin(angle1);
    float cos2 = cos(angle2);
    float sin2 = sin(angle2);

    for(int a = 0; a < shapes.size(); a++)
    {
        std::vector<ALine> aShape = shapes[a];
        int sides = aShape.size();
        std::vector<ALine> sRays;
        //std::vector<ARay> sRays;

        for(int b = 0; b < aShape.size(); b++)
        {
            ALine aLine = aShape[b];

            AVector dirVec = (aLine.GetPointB() - aLine.GetPointA()).Norm();
            AVector midPoint = (aLine.GetPointB() - aLine.GetPointA()) * 0.5 + aLine.GetPointA();

            // a right ray
            AVector dirVecRotated1;
            dirVecRotated1.x = cos1 * dirVec.x - sin1 * dirVec.y;
            dirVecRotated1.y = sin1 * dirVec.x + cos1 * dirVec.y;
            sRays.push_back(ALine(midPoint, dirVecRotated1.Norm(), true, b));
            //_rayLines.push_back(ALine(midPoint, midPoint + dirVecRotated1.Norm() * 0.25, true, b));

            // a left ray
            AVector dirVecRotated2;
            dirVecRotated2.x = cos2 * dirVec.x - sin2 * dirVec.y;
            dirVecRotated2.y = sin2 * dirVec.x + cos2 * dirVec.y;
            sRays.push_back(ALine(midPoint, dirVecRotated2.Norm(), false, b));
            //_rayLines.push_back(ALine(midPoint, midPoint + dirVecRotated2.Norm()  * 0.25, false, b));
        }


        // create combinations
        std::vector<std::pair<ALine, ALine>> rayCombination;
        for(int a = 0; a < sRays.size(); a++)
        {
            for(int b = a + 1; b < sRays.size(); b++)
            {
                if(sRays[a]._isRight != sRays[b]._isRight)
                {
                    std::pair<ALine, ALine> aPair(sRays[a], sRays[b]);
                    rayCombination.push_back(aPair);
                }
            }
        }

        // calculate candidates
        std::vector<std::pair<ALine, ALine>> lineCombination;
        for(int a = 0; a < rayCombination.size(); a++)
        {
            ALine rayA = rayCombination[a].first;
            ALine rayB = rayCombination[a].second;

            if(rayA._isRight == rayB._isRight) { continue; }
            if(rayA._side == rayB._side) { continue; }

            float dx = rayB.GetPointA().x - rayA.GetPointA().x;
            float dy = rayB.GetPointA().y - rayA.GetPointA().y;
            float det = rayB.GetPointB().x * rayA.GetPointB().y - rayB.GetPointB().y * rayA.GetPointB().x;
            float u = (dy * rayB.GetPointB().x - dx * rayB.GetPointB().y) / det;
            float v = (dy * rayA.GetPointB().x - dx * rayA.GetPointB().y) / det;

            // to do: this is weird
            //if((det > eps_val || det < -eps_val) && u > 0 && v > 0)

            /*
            if(u > 1000 || v > 1000)
            {
                std::cout << sides << "\n";
                std::cout << u << "\n";
                std::cout << v << "\n";
                std::cout << det << "\n";
                std::cout << "\n";
            }*/

            //if(det != 0 && u > 0 && v > 0 /*&& u < 1000 && v < 1000*/ )
            if((det > eps_val || det < -eps_val) && u > 0 && v > 0)
            {
                ALine aLine1(rayA.GetPointA(), rayA.GetPointA() + rayA.GetPointB() * u);
                aLine1._isRight = rayA._isRight;
                aLine1._side = rayA._side;


                ALine aLine2(rayB.GetPointA(), rayB.GetPointA() + rayB.GetPointB() * v);
                aLine2._isRight = rayB._isRight;
                aLine2._side = rayB._side;

                lineCombination.push_back(std::pair<ALine, ALine>(aLine1, aLine2));

                //std::cout << det << " " << aLine1.Magnitude() << "\n";
                //std::cout << det << " " << aLine2.Magnitude() << "\n";


            }
        }


        // select candidate
        std::sort (lineCombination.begin(), lineCombination.end(), LessThanLineMagnitudePair());
        int counter = 0;

        for(int i = 0; i < lineCombination.size(); i++)
        {
            ALine aLine1 = lineCombination[i].first;
            ALine aLine2 = lineCombination[i].second;

            //std::cout << aLine1.Magnitude() << " " << aLine1._isRight << " " << aLine1._side << "\n";
            //std::cout << aLine2.Magnitude() << " " << aLine2._isRight << " " << aLine2._side << "\n";
            //std::cout << aLine1.Magnitude() + aLine2.Magnitude() << "\n";
            //std::cout << "\n";
        }

        while(counter < sides)
        {
            //std::cout << sides << " " << counter << " " << "\n";
            ALine aLine1 = lineCombination[0].first;
            ALine aLine2 = lineCombination[0].second;
            _rayLines.push_back(aLine1);
            _rayLines.push_back(aLine2);
            //lineCombination.erase (lineCombination.begin());

            // remove first
            for(int i = lineCombination.size() - 1; i >= 0; i--)
            {
                std::pair<ALine, ALine> linePair = lineCombination[i];
                if(( linePair.first._isRight  ==  aLine1._isRight && linePair.first._side  == aLine1._side)    ||
                   ( linePair.second._isRight ==  aLine1._isRight && linePair.second._side == aLine1._side )   ||
                   ( linePair.first._isRight  ==  aLine2._isRight && linePair.first._side  == aLine2._side)    ||
                   ( linePair.second._isRight ==  aLine2._isRight && linePair.second._side == aLine2._side ))
                {
                    lineCombination.erase (lineCombination.begin() + i);
                }
            }

            counter++;
        }



    }
    PrepareLinesVAO2(_rayLines, &_rayLinesVbo, &_rayLinesVao);
}

void PatternGenerator::InitTiling()
{
    _shapes.clear();
    _tilings.clear();

    ReadXML("../IslamicStarPatterns/archimedeans.xml");
    ReadXML("../IslamicStarPatterns/hanbury.xml");

    // rendering-related
    _tilingLines.clear();

    // 4.4.4.4
    TilingData tilingData = GetTiling("3.3.4.3.4");
    //TilingData tilingData = GetTiling("4.4.4.4");

    AVector trans1 = tilingData._translation1;
    AVector trans2 = tilingData._translation2;
    AVector centerPt(this->_img_width / 2, this->_img_height / 2);
    //AVector centerPt(0, 0);
    //std::cout << "(" << trans1.x << ", " << trans1.y << ") (" << trans2.x << ", " << trans2.y << ")\n";
    for(int a = 0; a < 10; a++)
    {
        for(int b = 0; b < 10; b++)
        {
            for(int c = 0; c < tilingData._tiles.size(); c++)
            {
                TileData tileData = tilingData._tiles[c];
                //if(tileData._shapeType == ShapeType::S_REGULAR)
                //{
                float sides = tileData._sides;
                float radAngle = (M_PI / (float)sides);
                float radius = 1.0 / cos(radAngle);
                float angleOffset = tileData.GetRotation();

                /*
                if(tileData._shapeType == ShapeType::S_REGULAR && sides == 16)
                {
                    //std::cout << "5 angle offset " << angleOffset << "\n";
                    angleOffset = M_PI / sideDiv;
                    std::cout << sideDiv << "  " << angleOffset << "\n";
                }
                */

                std::vector<AVector> shape;
                if(tileData._shapeType == ShapeType::S_REGULAR)
                {
                    shape = GenerateNGon(sides, radius, angleOffset, AVector(0, 0));
                }
                else if(tileData._shapeType == ShapeType::S_POLYGON)
                {
                    shape = std::vector<AVector>(tileData._vertices);
                }

                for(int d = 0; d < tileData._transforms.size(); d++)
                {
                    QMatrix3x3 mat = tileData._transforms[d];
                    std::vector<AVector> tempShape(shape);
                    MultiplyShape(mat, tempShape);

                    AVector pos = centerPt + trans1 * a + trans2 * b;
                    for(int i = 0; i < tempShape.size(); i++)
                        { tempShape[i] += pos; }

                    //if(tileData._shapeType == ShapeType::S_POLYGON)
                    //{
                    ConcatNGon(tempShape, _tilingLines);
                    ConcatShapes(tempShape, _shapes);
                    //}
                }
                //std::vector<AVector> shape = GenerateNGon(sides, radius, angleOffset, AVector(0, 0));
                //MultiplyShape(tileData.);
                //}
            }
        }
    }

    InferenceAlgorithm(_shapes);

    PrepareLinesVAO1(_tilingLines, &_tilingLinesVbo, &_tilingLinesVao, QVector3D(1.0, 0.0, 0.0));
}

void PatternGenerator::Paint()
{
    _shaderProgram->setUniformValue(_use_color_location, (GLfloat)1.0);

    if(_tilingLines.size() != 0)
    {
        glLineWidth(1.0f);
        _tilingLinesVao.bind();
        glDrawArrays(GL_LINES, 0, _tilingLines.size() * 2);
        _tilingLinesVao.release();
    }

    if(_rayLines.size() != 0)
    {
        glLineWidth(3.0f);
        _rayLinesVao.bind();
        glDrawArrays(GL_LINES, 0, _rayLines.size() * 2);
        _rayLinesVao.release();
    }
}

void PatternGenerator::ConcatNGon(std::vector<AVector> sourcePolygon, std::vector<ALine> &destinationLines)
{
    for(uint a = 0; a < sourcePolygon.size(); a++)
    {
        destinationLines.push_back(ALine(sourcePolygon[a],
                                         sourcePolygon[(a + 1) % sourcePolygon.size()]));
    }
}

void PatternGenerator::ConcatShapes(std::vector<AVector> sourcePolygon, std::vector<std::vector<ALine>> &shapes)
{
    std::vector<ALine> tempShape;
    for(uint a = 0; a < sourcePolygon.size(); a++)
    {
        tempShape.push_back(ALine(sourcePolygon[a],
                              sourcePolygon[(a + 1) % sourcePolygon.size()]));
    }
    shapes.push_back(tempShape);

}

void PatternGenerator::MultiplyShape(QMatrix3x3 mat, std::vector<AVector>& shape)
{
    for(int a = 0; a < shape.size(); a++)
        { shape[a] = MultiplyVector(mat, shape[a]); }
}

AVector PatternGenerator::MultiplyVector(QMatrix3x3 mat, AVector vec)
{
    float x = mat(0, 0) * vec.x + mat(0, 1) * vec.y + mat(0, 2) * 1.0;
    float y = mat(1, 0) * vec.x + mat(1, 1) * vec.y + mat(1, 2) * 1.0;
    //float z = mat(2, 0) * vec.x + mat(2, 1) * vec.y + mat(2, 2) * 1.0;
    return AVector(x, y);
}

std::vector<AVector> PatternGenerator::GenerateNGon(float sides, float radius, float angleOffset, AVector centerPt)
{
    std::vector<AVector> shape;
    float addValue = M_PI * 2.0 / sides;
    float valLimit = M_PI * 2.0 + angleOffset;
    float epsilonLimit = std::numeric_limits<float>::epsilon() * 1000;
    for(float a = angleOffset; (valLimit - a) > epsilonLimit; a += addValue)
    {
        float xPt = centerPt.x + radius * sin(a);
        float yPt = centerPt.y + radius * cos(a);
        shape.push_back(AVector(xPt, yPt));
    }

    //std::cout << "GenerateNGon " << shape[shape.size() - 1].Distance(shape[0]) << "\n";
    //std::cout << "GenerateNGon " << shape.size() << "\n";
    int intSides = (int)sides;
    if(intSides != shape.size())
    {
        std::cout << "GenerateNGon error\n";
    }

    return shape;
}

void PatternGenerator::PrepareLinesVAO2(std::vector<ALine> lines, QOpenGLBuffer* linesVbo, QOpenGLVertexArrayObject* linesVao)
{
    if(linesVao->isCreated()) { linesVao->destroy(); }

    linesVao->create();
    linesVao->bind();

    QVector<VertexData> data;
    QVector3D vecCol;
    for(uint a = 0; a < lines.size(); a++)
    {
        if(a % 2 == 0)
        {
            float r = (float)(rand() % 255) / 255.0;
            float g = (float)(rand() % 255) / 255.0;
            float b = (float)(rand() % 255) / 255.0;
            vecCol = QVector3D(r, g, b);
        }
        data.append(VertexData(QVector3D(lines[a].XA, lines[a].YA,  0), QVector2D(), vecCol));
        data.append(VertexData(QVector3D(lines[a].XB, lines[a].YB,  0), QVector2D(), vecCol));
    }

    linesVbo->create();
    linesVbo->bind();
    linesVbo->allocate(data.data(), data.size() * sizeof(VertexData));

    quintptr offset = 0;

    _shaderProgram->enableAttributeArray(_vertexLocation);
    _shaderProgram->setAttributeBuffer(_vertexLocation, GL_FLOAT, 0, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);
    offset += sizeof(QVector2D);

    _shaderProgram->enableAttributeArray(_colorLocation);
    _shaderProgram->setAttributeBuffer(_colorLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    linesVao->release();
}


void PatternGenerator::PrepareLinesVAO1(std::vector<ALine> lines, QOpenGLBuffer* linesVbo, QOpenGLVertexArrayObject* linesVao, QVector3D vecCol)
{
    if(linesVao->isCreated()) { linesVao->destroy(); }

    linesVao->create();
    linesVao->bind();

    QVector<VertexData> data;
    for(uint a = 0; a < lines.size(); a++)
    {
        data.append(VertexData(QVector3D(lines[a].XA, lines[a].YA,  0), QVector2D(), vecCol));
        data.append(VertexData(QVector3D(lines[a].XB, lines[a].YB,  0), QVector2D(), vecCol));
    }

    linesVbo->create();
    linesVbo->bind();
    linesVbo->allocate(data.data(), data.size() * sizeof(VertexData));

    quintptr offset = 0;

    _shaderProgram->enableAttributeArray(_vertexLocation);
    _shaderProgram->setAttributeBuffer(_vertexLocation, GL_FLOAT, 0, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);
    offset += sizeof(QVector2D);

    _shaderProgram->enableAttributeArray(_colorLocation);
    _shaderProgram->setAttributeBuffer(_colorLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    linesVao->release();
}
