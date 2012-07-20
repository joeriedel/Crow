//
//  Shader.fsh
//  CrowiOS
//
//  Created by Joseph Riedel on 7/19/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
