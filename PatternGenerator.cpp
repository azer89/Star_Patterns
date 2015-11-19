
#include <stdlib.h>
#include <iostream>
#include <algorithm>

#include "tinyxml2.h"

//#include "ARay.h"
#include "VertexData.h"
#include "PatternGenerator.h"
#include "SystemParams.h"
#include "Triangulator.h"


PatternGenerator::PatternGenerator()
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
        //std::cout << tilingData._name <<  "\n";

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
    _tempPoints.clear();
    _triangleLines.clear();
    _backTriangleLines.clear();
    _addTriangleLines.clear();
    _tempLines.clear();
    _uLines.clear();
    _oLines.clear();

    float eps_val = std::numeric_limits<float>::epsilon() * 1000.0f;

    float angle1 = -SystemParams::rad_angle;
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

        for(int b = 0; b < aShape.size(); b++)
        {
            ALine aLine = aShape[b];

            AVector dirVec = (aLine.GetPointB() - aLine.GetPointA()).Norm();
            AVector midPoint = (aLine.GetPointB() - aLine.GetPointA()) * 0.5 + aLine.GetPointA();

            // a right ray
            AVector dirVecRotated1;
            dirVecRotated1.x = cos1 * dirVec.x - sin1 * dirVec.y;
            dirVecRotated1.y = sin1 * dirVec.x + cos1 * dirVec.y;
            sRays.push_back(ALine(midPoint + dirVec * SystemParams::contact_delta,
                                  dirVecRotated1.Norm(), true, b));

            // a left ray
            AVector dirVecRotated2;
            dirVecRotated2.x = cos2 * dirVec.x - sin2 * dirVec.y;
            dirVecRotated2.y = sin2 * dirVec.x + cos2 * dirVec.y;
            sRays.push_back(ALine(midPoint + AVector(-dirVec.x, -dirVec.y) * SystemParams::contact_delta,
                                  dirVecRotated2.Norm(), false, b));
        }

        // create combinations
        std::vector<std::pair<ALine, ALine>> rayCombination;
        for(int a = 0; a < sRays.size(); a++)
        {
            for(int b = a + 1; b < sRays.size(); b++)
            {
                ALine line1 = sRays[a];
                ALine line2 = sRays[b];

                if(line1._isRight)
                {
                    std::pair<ALine, ALine> aPair(line1, line2);
                    rayCombination.push_back(aPair);
                }
                else // flip
                {
                    std::pair<ALine, ALine> aPair(line2, line1);
                    rayCombination.push_back(aPair);
                }
            }
        }

        // calculate candidates
        std::vector<std::pair<ALine, ALine>> lineCombination1;
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


            if((det > eps_val || det < -eps_val) && u > 0 && v > 0)
            {
                ALine aLine1(rayA.GetPointA(), rayA.GetPointA() + rayA.GetPointB() * u);
                aLine1._isRight = rayA._isRight;
                aLine1._side = rayA._side;

                ALine aLine2(rayB.GetPointA(), rayB.GetPointA() + rayB.GetPointB() * v);
                aLine2._isRight = rayB._isRight;
                aLine2._side = rayB._side;

                lineCombination1.push_back(std::pair<ALine, ALine>(aLine1, aLine2));
            }
            else if(CheckCollinearCase(rayA, rayB) || CheckHorizontalVerticalCase(rayA, rayB))
            {
                AVector midPoint = rayA.GetPointA() + (rayB.GetPointA() - rayA.GetPointA()) * 0.5f;

                ALine aLine1(rayA.GetPointA(), midPoint);
                aLine1._isRight = rayA._isRight;
                aLine1._side = rayA._side;

                ALine aLine2(rayB.GetPointA(), midPoint);
                aLine2._isRight = rayB._isRight;
                aLine2._side = rayB._side;

                lineCombination1.push_back(std::pair<ALine, ALine>(aLine1, aLine2));
            }
        }

        std::sort (lineCombination1.begin(), lineCombination1.end(), LessThanLineMagnitudePair());

        int counter = 0;
        std::vector<std::pair<ALine, ALine>> lineCombination2;
        while(counter < sides)
        {

            ALine aLine1 = lineCombination1[0].first;
            ALine aLine2 = lineCombination1[0].second;

            lineCombination2.push_back(lineCombination1[0]);

            for(int i = lineCombination1.size() - 1; i >= 0; i--)
            {
                std::pair<ALine, ALine> linePair = lineCombination1[i];
                if(( linePair.first._isRight  ==  aLine1._isRight && linePair.first._side  == aLine1._side)    ||
                   ( linePair.second._isRight ==  aLine1._isRight && linePair.second._side == aLine1._side )   ||

                   ( linePair.first._isRight  ==  aLine2._isRight && linePair.first._side  == aLine2._side)    ||
                   ( linePair.second._isRight ==  aLine2._isRight && linePair.second._side == aLine2._side ))
                {
                    lineCombination1.erase (lineCombination1.begin() + i);
                }
            }

            counter++;
        }


        for(int a = 0; a < lineCombination2.size(); a++)
        {
            ALine aLine1 = lineCombination2[a].first;
            ALine aLine2 = lineCombination2[a].second;
            _rayLines.push_back(aLine1);
            _rayLines.push_back(aLine2);

            CalculateInterlace(lineCombination2[a], aShape, _uLines, _oLines);
        }

        if(SystemParams::contact_delta > -eps_val && SystemParams::contact_delta < eps_val)
        {
            std::vector<ALine> triangles1 = Triangulator::GetTriangles1(lineCombination2, aShape[0].GetPointA());

            _triangleLines.insert( _triangleLines.end(), triangles1.begin(), triangles1.end() );
        }
        else
        {
            std::vector<ALine> triangles1 = Triangulator::GetTriangles2(aShape, AVector(-10, -10));
            std::vector<ALine> triangles2 = Triangulator::GetTriangles3(lineCombination2, aShape);
            std::vector<ALine> triangles3 = Triangulator::GetTriangles4(lineCombination2, aShape);

            _triangleLines.insert( _triangleLines.end(), triangles1.begin(), triangles1.end() );
            _addTriangleLines.insert( _addTriangleLines.end(), triangles3.begin(), triangles3.end() );
            _backTriangleLines.insert( _backTriangleLines.end(), triangles2.begin(), triangles2.end() );
        }
    }

    // debug
    //_tempLines.insert( _tempLines.end(), _triangleLines.begin(), _triangleLines.end() );
    //_tempLines.insert( _tempLines.end(), _addTriangleLines.begin(), _addTriangleLines.end() );
    //_tempLines.insert( _tempLines.end(), _backTriangleLines.begin(), _backTriangleLines.end() );
    //PrepareLinesVAO1(_tempLines, &_tempLinesVbo, &_tempLinesVao, QVector3D(0, 1, 0));

    BuildTrianglesVertexData(_triangleLines, &_trianglesVbo, &_trianglesVao, SystemParams::star_color);
    BuildTrianglesVertexData(_addTriangleLines, &_addTrianglesVbo, &_addTrianglesVao, SystemParams::star_color);
    BuildTrianglesVertexData(_backTriangleLines, &_backTrianglesVbo, &_backTrianglesVao, SystemParams::background_color);
    BuildQuadsVertexData(_uLines, &_uQuadsVbo, &_uQuadsVao, SystemParams::ribbon_color);
    BuildQuadsVertexData(_oLines, &_oQuadsVbo, &_oQuadsVao, SystemParams::ribbon_color);
    BuildLinesVertexData1(_uLines, &_uLinesVbo, &_uLinesVao, SystemParams::interlacing_color);
    BuildLinesVertexData1(_oLines, &_oLinesVbo, &_oLinesVao, SystemParams::interlacing_color);
}

void PatternGenerator::CalculateInterlace(std::pair<ALine, ALine> segment, std::vector<ALine> aShape, std::vector<ALine> &uLines, std::vector<ALine> &oLines)
{
    ALine aLine1 = segment.first;
    ALine aLine2 = segment.second;
    AVector dir1 = aLine1.Direction().Norm();
    AVector dir2 = aLine2.Direction().Norm();

    float halfWidth = SystemParams::ribbon_width * 0.5f;
    float eps_val = std::numeric_limits<float>::epsilon() * 1000;

    AVector cPt;
    AVector dPt;

    // parallel test
    float par_val = abs( dir1.Dot(dir2) / (dir1.Length() * dir2.Length()) );
    if(par_val > 1.0f - eps_val)
    {
        dir1 = AVector(-dir1.y, dir1.x);
        dir2 = AVector(-dir2.y, dir2.x);

        AVector midDir = dir1;
        float hypotenuse1 = halfWidth;

        midDir *= hypotenuse1;

        AVector midPt = aLine1.GetPointB();
        cPt = midPt - midDir;
        dPt = midPt + midDir;
    }
    else
    {
        AVector dir1 = aLine1.Direction().Norm();
        AVector dir2 = aLine2.Direction().Norm();
        AVector midDir = (dir1 + dir2) / 2.0f;

        float angle1 = AngleInBetween(dir1, dir2) / 2.0f; // half angle because intersection
        float angle2 = M_PI / 2.0f - angle1;  // another angle
        float hypotenuse1 = halfWidth / cos(angle2);

        midDir = midDir.Norm();
        midDir *= hypotenuse1;

        AVector midPt = aLine1.GetPointB();

        float dir_angle = GetRotation(dir1, dir2);
        if(dir_angle < 0)
        {
            cPt = midPt + midDir;
            dPt = midPt - midDir;
        }
        else
        {
            cPt = midPt - midDir;
            dPt = midPt + midDir;
        }
    }

    // start
    float angleA = SystemParams::rad_angle;
    float angleB = M_PI / 2.0f - angleA;
    float hypotenuse2 = halfWidth / cos(angleB);
    AVector dirHA = aShape[aLine1._side].Direction().Norm();
    AVector dirVA = AVector(dirHA.y, -dirHA.x);
    AVector aPt = aLine1.GetPointA() + dirVA * hypotenuse2;
    AVector bPt = aLine1.GetPointA() + dirHA * hypotenuse2;
    AVector bPtInv = aLine1.GetPointA() + dirHA.Inverse() * hypotenuse2;

    // end
    float angleX = M_PI - SystemParams::rad_angle;
    float angleY = M_PI / 2.0f - angleX;
    float hypotenuse3 = halfWidth / cos(angleY);
    AVector dirX = aShape[aLine2._side].Direction().Norm();
    AVector dirVB = AVector(dirX.y, -dirX.x);
    AVector dirHB = AVector(-dirX.x, -dirX.y);
    AVector ePt = aLine2.GetPointA() + dirVB * hypotenuse3;
    AVector fPt = aLine2.GetPointA() + dirHB * hypotenuse3;
    AVector fPtInv = aLine2.GetPointA() + dirHB.Inverse() * hypotenuse3;

    // under
    //uLines.push_back(ALine(aPt, cPt));    // non interlace
    uLines.push_back(ALine(bPtInv, cPt));   // interlace
    uLines.push_back(ALine(bPt, dPt));

    // over
    //oLines.push_back(ALine(cPt, ePt)); // non interlace
    oLines.push_back(ALine(cPt, fPtInv)); // interlace
    oLines.push_back(ALine(dPt, fPt));
}

// signed
float PatternGenerator::GetRotation(AVector pt1, AVector pt2)
{
    float perpDot = pt1.x * pt2.y - pt1.y * pt2.x;
    return (float)atan2(perpDot, pt1.Dot(pt2));
}

// unsigned
float PatternGenerator::AngleInBetween(AVector vec1, AVector vec2)
{
    //std::cout << vec1.Dot(vec2) << " " << vec1.Length() << " " << vec2.Length() << "\n";
    return acos(vec1.Dot(vec2) / (vec1.Length() * vec2.Length()));
}

bool PatternGenerator::CheckHorizontalVerticalCase(ALine ray1, ALine ray2)
{
    float eps_val = std::numeric_limits<float>::epsilon() * 1000;
    AVector midPoint = ray1.GetPointA() + (ray2.GetPointA() - ray1.GetPointA()) * 0.5f;

    float u1 = (midPoint.x - ray1.GetPointA().x) / ray1.GetPointB().x;
    float u2 = (midPoint.y - ray1.GetPointA().y) / ray1.GetPointB().y;
    float v1 = (midPoint.x - ray2.GetPointA().x) / ray2.GetPointB().x;
    float v2 = (midPoint.y - ray2.GetPointA().y) / ray2.GetPointB().y;

    // vertical case
    if(abs(ray1.GetPointB().x) < eps_val && abs(ray2.GetPointB().x) <  eps_val &&
       ray1.GetPointB().y != 0 && ray2.GetPointB().y != 0 &&
       u2 > 0 && v2 > 0 )
    {
        return true;
    }
    // horizontal case
    else if(ray1.GetPointB().x != 0 && ray2.GetPointB().x != 0 &&
            abs(ray1.GetPointB().y) < eps_val && abs(ray2.GetPointB().y) < eps_val &&
            u1 > 0 && v1 > 0)
    {
        return true;
    }
    return false;
}

// to do: this function might be wrong
bool PatternGenerator::CheckCollinearCase(ALine ray1, ALine ray2)
{
    //std::cout << "CheckCollinearCase\n";

    float eps_val = std::numeric_limits<float>::epsilon() * 1000;

    AVector midPoint = ray1.GetPointA() + (ray2.GetPointA() - ray1.GetPointA()) * 0.5f;

    float u1 = (midPoint.x - ray1.GetPointA().x) / ray1.GetPointB().x;
    float u2 = (midPoint.y - ray1.GetPointA().y) / ray1.GetPointB().y;
    float v1 = (midPoint.x - ray2.GetPointA().x) / ray2.GetPointB().x;
    float v2 = (midPoint.y - ray2.GetPointA().y) / ray2.GetPointB().y;

    if(abs(u1 - u2) < eps_val && abs(v1 - v2) < eps_val && u1 > 0 && u2 > 0 && v1 > 0 && v2 > 0)
    {
        return true;
    }

    return false;
}


void PatternGenerator::GeneratePattern(std::string tilingName)
{
    _tilingLines.clear();
    _shapes.clear();

    TilingData tilingData = GetTiling(tilingName);

    AVector trans1 = tilingData._translation1;
    AVector trans2 = tilingData._translation2;
    AVector centerPt(this->_img_width / 2, this->_img_height / 2);

    for(int a = 0; a < SystemParams::w; a++)
    {
        for(int b = 0; b < SystemParams::h; b++)
        {
            for(int c = 0; c < tilingData._tiles.size(); c++)
            {
                TileData tileData = tilingData._tiles[c];

                float sides = tileData._sides;
                float radAngle = (M_PI / (float)sides);
                float radius = 1.0 / cos(radAngle);
                float angleOffset = tileData.GetRotation();

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

                    ConcatNGon(tempShape, _tilingLines);
                    ConcatShapes(tempShape, _shapes);
                }
            }
        }
    }
    InferenceAlgorithm(_shapes);
    BuildLinesVertexData1(_tilingLines, &_tilingLinesVbo, &_tilingLinesVao, QVector3D(1.0, 0.0, 0.0));
}

void PatternGenerator::InitTiling()
{

    _tilings.clear();

    ReadXML("../IslamicStarPatterns/archimedeans.xml");
    ReadXML("../IslamicStarPatterns/hanbury.xml");


}

void PatternGenerator::Paint(float zoomFactor)
{
    _shaderProgram->setUniformValue(_use_color_location, (GLfloat)1.0);

    if(_tempLines.size() != 0)
    {
        glLineWidth(1.0f);
        _tempLinesVao.bind();
        glDrawArrays(GL_LINES, 0, _tempLines.size() * 2);
        _tempLinesVao.release();
    }

    if(_tempPointsVao.isCreated())
    {
        glPointSize(10.0f);
        _tempPointsVao.bind();
        glDrawArrays(GL_POINTS, 0, _tempPoints.size());
        _tempPointsVao.release();
    }

    if(_uLinesVao.isCreated())
    {
        _shaderProgram->setUniformValue(_use_color_location, (GLfloat)1.0);
        _uLinesVao.bind();
        glLineWidth(SystemParams::line_width  * zoomFactor);
        glDrawArrays(GL_LINES, 0, _uLines.size() * 2);
        _uLinesVao.release();
    }

    if(_uQuadsVao.isCreated())
    {
        _shaderProgram->setUniformValue(_use_color_location, (GLfloat)1.0);
        _uQuadsVao.bind();
        glDrawArrays(GL_QUADS, 0, _uLines.size() * 2);
        _uQuadsVao.release();
    }

    if(_oLinesVao.isCreated())
    {
        _shaderProgram->setUniformValue(_use_color_location, (GLfloat)1.0);
        _oLinesVao.bind();
        glLineWidth(SystemParams::line_width  * zoomFactor);
        glDrawArrays(GL_LINES, 0, _oLines.size() * 2);
        _oLinesVao.release();
    }

    if(_oQuadsVao.isCreated())
    {
        _shaderProgram->setUniformValue(_use_color_location, (GLfloat)1.0);
        _oQuadsVao.bind();
        glDrawArrays(GL_QUADS, 0, _oLines.size() * 2);
        _oQuadsVao.release();
    }



    if(_tilingLines.size() != 0 && SystemParams::show_tiling)
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

    if(_addTrianglesVao.isCreated())
    {
        _addTrianglesVao.bind();
        glDrawArrays(GL_TRIANGLES, 0, _addTriangleLines.size());
        _addTrianglesVao.release();
    }

    if(_backTrianglesVao.isCreated())
    {
        _backTrianglesVao.bind();
        glDrawArrays(GL_TRIANGLES, 0, _backTriangleLines.size());
        _backTrianglesVao.release();
    }

    if(_trianglesVao.isCreated())
    {
        _trianglesVao.bind();
        glDrawArrays(GL_TRIANGLES, 0, _triangleLines.size());
        _trianglesVao.release();
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

    int intSides = (int)sides;
    if(intSides != shape.size())
    {
        std::cout << "GenerateNGon error\n";
    }

    return shape;
}

void PatternGenerator::BuildLinesVertexData2(std::vector<ALine> lines, QOpenGLBuffer* linesVbo, QOpenGLVertexArrayObject* linesVao)
{
    if(linesVao->isCreated()) { linesVao->destroy(); }

    linesVao->create();
    linesVao->bind();

    QVector<VertexData> data;
    //QVector3D vecCol;

    QVector3D vecCol1(1, 1, 0);
    QVector3D vecCol2(0, 0, 1);

    for(uint a = 0; a < lines.size(); a++)
    {
        if(a % 2 == 0)
        {
            //float r = (float)(rand() % 255) / 255.0;
            //float g = (float)(rand() % 255) / 255.0;
            //float b = (float)(rand() % 255) / 255.0;
            //vecCol = QVector3D(r, g, b);
        }
        //data.append(VertexData(QVector3D(lines[a].XA, lines[a].YA,  0), QVector2D(), vecCol));
        //data.append(VertexData(QVector3D(lines[a].XB, lines[a].YB,  0), QVector2D(), vecCol));

        data.append(VertexData(QVector3D(lines[a].XA, lines[a].YA,  0), QVector2D(), vecCol1));
        data.append(VertexData(QVector3D(lines[a].XB, lines[a].YB,  0), QVector2D(), vecCol2));
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

void PatternGenerator::BuildLinesVertexData0(std::vector<ALine> lines, QOpenGLBuffer* linesVbo, QOpenGLVertexArrayObject* linesVao, QVector3D vecCol1, QVector3D vecCol2)
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
            vecCol = vecCol1;
        }
        else
        {
            vecCol = vecCol2;
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


void PatternGenerator::BuildLinesVertexData1(std::vector<ALine> lines, QOpenGLBuffer* linesVbo, QOpenGLVertexArrayObject* linesVao, QVector3D vecCol)
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

void PatternGenerator::BuildQuadsVertexData(std::vector<ALine> lines, QOpenGLBuffer* vbo, QOpenGLVertexArrayObject* vao, QVector3D vecCol)
{
    if(vao->isCreated()) { vao->destroy(); }

    vao->create();
    vao->bind();

    QVector<VertexData> data;
    for(uint a = 0; a < lines.size() - 1; a += 2)
    {
        data.append(VertexData(QVector3D(lines[a].XA, lines[a].YA,  0), QVector2D(), vecCol));
        data.append(VertexData(QVector3D(lines[a].XB, lines[a].YB,  0), QVector2D(), vecCol));

        data.append(VertexData(QVector3D(lines[a+1].XB, lines[a+1].YB,  0), QVector2D(), vecCol));
        data.append(VertexData(QVector3D(lines[a+1].XA, lines[a+1].YA,  0), QVector2D(), vecCol));

    }

    vbo->create();
    vbo->bind();
    vbo->allocate(data.data(), data.size() * sizeof(VertexData));

    quintptr offset = 0;

    _shaderProgram->enableAttributeArray(_vertexLocation);
    _shaderProgram->setAttributeBuffer(_vertexLocation, GL_FLOAT, 0, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);
    offset += sizeof(QVector2D);

    _shaderProgram->enableAttributeArray(_colorLocation);
    _shaderProgram->setAttributeBuffer(_colorLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    vao->release();
}

void PatternGenerator::BuildPointsVertexData(std::vector<AVector> points, QOpenGLBuffer* ptsVbo, QOpenGLVertexArrayObject* ptsVao, QVector3D vecCol)
{
    if(ptsVao->isCreated())
    {
        ptsVao->destroy();
    }

    ptsVao->create();
    ptsVao->bind();

    QVector<VertexData> data;
    for(size_t a = 0; a < points.size(); a++)
    {
        data.append(VertexData(QVector3D(points[a].x, points[a].y,  0), QVector2D(), vecCol));
    }

    ptsVbo->create();
    ptsVbo->bind();
    ptsVbo->allocate(data.data(), data.size() * sizeof(VertexData));

    quintptr offset = 0;

    int vertexLocation = _shaderProgram->attributeLocation("vert");
    _shaderProgram->enableAttributeArray(vertexLocation);
    _shaderProgram->setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);
    offset += sizeof(QVector2D);

    _shaderProgram->enableAttributeArray(_colorLocation);
    _shaderProgram->setAttributeBuffer(_colorLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    ptsVao->release();
}

void PatternGenerator::BuildTrianglesVertexData(std::vector<ALine> lines, QOpenGLBuffer* vbo, QOpenGLVertexArrayObject* vao, QVector3D vecCol)
{
    if(vao->isCreated()) { vao->destroy(); }

    vao->create();
    vao->bind();

    QVector<VertexData> data;
    for(uint a = 0; a < lines.size(); a += 3)
    {
        ALine line1 = lines[a];
        ALine line2 = lines[a + 1];
        ALine line3 = lines[a + 2];

        /*
            triangles.push_back(ALine(v1, v2));
            triangles.push_back(ALine(v2, v3));
            triangles.push_back(ALine(v3, v1));
        */

        data.append(VertexData(QVector3D(line1.XA, line1.YA,  0), QVector2D(), vecCol));
        data.append(VertexData(QVector3D(line1.XB, line1.YB,  0), QVector2D(), vecCol));
        data.append(VertexData(QVector3D(line3.XA, line3.YA,  0), QVector2D(), vecCol));

        //data.append(VertexData(QVector3D(lines[a].XA, lines[a].YA,  0), QVector2D(), vecCol));
        //data.append(VertexData(QVector3D(lines[a].XB, lines[a].YB,  0), QVector2D(), vecCol));
        //data.append(VertexData(QVector3D(lines[a+1].XB, lines[a+1].YB,  0), QVector2D(), vecCol));
        //data.append(VertexData(QVector3D(lines[a+1].XA, lines[a+1].YA,  0), QVector2D(), vecCol));

    }

    vbo->create();
    vbo->bind();
    vbo->allocate(data.data(), data.size() * sizeof(VertexData));

    quintptr offset = 0;

    _shaderProgram->enableAttributeArray(_vertexLocation);
    _shaderProgram->setAttributeBuffer(_vertexLocation, GL_FLOAT, 0, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);
    offset += sizeof(QVector2D);

    _shaderProgram->enableAttributeArray(_colorLocation);
    _shaderProgram->setAttributeBuffer(_colorLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    vao->release();
}

AVector PatternGenerator::GetPolygonCentroid(std::vector<ALine> shapes)
{
    AVector centroid(0, 0);
    float signedArea = 0.0f;
    //double x0 = 0.0; // Current vertex X
    //double y0 = 0.0; // Current vertex Y
    //double x1 = 0.0; // Next vertex X
    //double y1 = 0.0; // Next vertex Y
    //double a = 0.0;  // Partial signed area

    // For all vertices except last
    //int i=0;
    for (int i = 0; i < shapes.size(); i++)
    {
        float x0 = shapes[i].GetPointA().x;
        float y0 = shapes[i].GetPointA().y;
        float x1 = shapes[i].GetPointB().x;
        float y1 = shapes[i].GetPointB().y;
        float a = x0 * y1 - x1 * y0;
        signedArea += a;
        centroid.x += (x0 + x1) * a;
        centroid.y += (y0 + y1) * a;
    }

    // Do last vertex
    //x0 = vertices[i].x;
    //y0 = vertices[i].y;
    //x1 = vertices[0].x;
    //y1 = vertices[0].y;
    //a = x0*y1 - x1*y0;
    //signedArea += a;
    //centroid.x += (x0 + x1)*a;
    //centroid.y += (y0 + y1)*a;

    signedArea *= 0.5;
    centroid.x /= (6.0f * signedArea);
    centroid.y /= (6.0f * signedArea);

    return centroid;
}
