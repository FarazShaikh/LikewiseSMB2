/* Editor Settings: expandtabs and use 4 spaces for indentation
 * ex: set softtabstop=4 tabstop=8 expandtab shiftwidth=4: *
 * -*- mode: c, c-basic-offset: 4 -*- */

/*
 * Copyright Likewise Software    2004-2008
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.  You should have received a copy of the GNU General
 * Public License along with this program.  If not, see 
 * <http://www.gnu.org/licenses/>.
 *
 * LIKEWISE SOFTWARE MAKES THIS SOFTWARE AVAILABLE UNDER OTHER LICENSING
 * TERMS AS WELL.  IF YOU HAVE ENTERED INTO A SEPARATE LICENSE AGREEMENT
 * WITH LIKEWISE SOFTWARE, THEN YOU MAY ELECT TO USE THE SOFTWARE UNDER THE
 * TERMS OF THAT SOFTWARE LICENSE AGREEMENT INSTEAD OF THE TERMS OF THE GNU
 * GENERAL PUBLIC LICENSE, NOTWITHSTANDING THE ABOVE NOTICE.  IF YOU
 * HAVE QUESTIONS, OR WISH TO REQUEST A COPY OF THE ALTERNATE LICENSING
 * TERMS OFFERED BY LIKEWISE SOFTWARE, PLEASE CONTACT LIKEWISE SOFTWARE AT
 * license@likewisesoftware.com
 */

/*
 *  main.h
 *  DomainJoin
 *
 *  Created by Sriram Nambakam on 8/9/07.
 *  Copyright 2007 Centeris Corporation. All rights reserved.
 *
 */
#ifndef __MAIN_H__
#define __MAIN_H__

#include <Carbon/Carbon.h>
#include <Security/Security.h>

#include "DomainMigrateWindow.h"
#include "DomainJoinWindow.h"
#include "DomainLeaveWindow.h"
#include "MainWindow.h"

class DomainJoinApp : public TApplication
{
    public:
							
		DomainJoinApp();
        virtual ~DomainJoinApp();
		
	protected:
	    
		DomainJoinApp(const DomainJoinApp& other);
		DomainJoinApp& operator=(const DomainJoinApp& other);
	
	public:
	
	    virtual void Run();
        
    protected:
	
        virtual Boolean     HandleCommand( const HICommandExtended& inCommand );
		
		void JoinOrLeaveDomain();
        void MigrateUser();
		
		DomainJoinWindow& GetJoinWindow();
		DomainLeaveWindow& GetLeaveWindow();
        DomainMigrateWindow& GetMigrateWindow();
        
        void EnsureAuthorization();
		
		void FixProcessEnvironment();
		
	protected:
	
	    static const int ApplicationSignature;
		
	private:
	
	    MainWindow*          _mainWindow;
	    DomainJoinWindow*    _joinWindow;
		DomainLeaveWindow*   _leaveWindow;
        DomainMigrateWindow* _migrateWindow;
		
		char* _envPath;
};

#endif /* __MAIN_H__ */
