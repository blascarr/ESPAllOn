#ifndef _ESPALLON_PROJECTS_H
#define _ESPALLON_PROJECTS_H

#include "../../utils.h"
#include "../ProjectsAPIClient.h"
#include <ESPUI.h>
/**
 * Simple Projects endpoint handler for ESPAllOn system
 * Provides web interface for project management and configuration loading
 */
class ESPAllOnProjects {
  public:
	/**
	 * Register the projects endpoints with the web server
	 */
	static void registerProjectsEndpoints() {
		// Main projects page
		ESPUI.server->on("/projects", HTTP_GET, handleProjectsPageRequest);

		// API endpoints
		ESPUI.server->on("/api/projects", HTTP_GET, handleProjectsAPIRequest);
		ESPUI.server->on("/api/project/*/load", HTTP_POST,
						 handleLoadProjectRequest);

		DUMPSLN("Projects endpoints registered");
	}

  private:
	/**
	 * HTTP request handler for the projects page
	 * @param request AsyncWebServerRequest object
	 */
	static void handleProjectsPageRequest(AsyncWebServerRequest *request) {
		DUMPSLN("Serving projects page");

		// Serve simple HTML page
		String html = F("<!DOCTYPE html><html><head><meta charset='UTF-8'>");
		html += F("<title>ESPAllOn - Proyectos</title>");
		html += F("<style>");
		html += F("body{font-family:Arial,sans-serif;margin:20px;background:#"
				  "f5f5f5}");
		html += F(".container{max-width:800px;margin:0 "
				  "auto;background:white;padding:20px;border-radius:10px;box-"
				  "shadow:0 2px 10px rgba(0,0,0,0.1)}");
		html += F("h1{color:#2c3e50;text-align:center}");
		html += F(".btn{padding:10px "
				  "20px;margin:10px;border:none;border-radius:5px;cursor:"
				  "pointer;font-size:16px}");
		html += F(".btn-primary{background:#3498db;color:white}");
		html += F(".btn-success{background:#27ae60;color:white}");
		html += F(".btn:disabled{opacity:0.6;cursor:not-allowed}");
		html += F(".status{padding:10px;margin:10px "
				  "0;border-radius:5px;text-align:center}");
		html += F(".status.loading{background:#fff3cd;color:#856404}");
		html += F(".status.success{background:#d4edda;color:#155724}");
		html += F(".status.error{background:#f8d7da;color:#721c24}");
		html += F(".projects{margin-top:20px}");
		html += F(".project{border:1px solid #ddd;margin:10px "
				  "0;padding:15px;border-radius:5px;cursor:pointer}");
		html += F(".project:hover{background:#f8f9fa}");
		html += F(".project.selected{border-color:#27ae60;background:#f8fff9}");
		html += F(".project-title{font-weight:bold;color:#2c3e50}");
		html += F(".project-description{color:#7f8c8d;margin:5px 0}");
		html += F("</style></head><body>");

		html += F("<div class='container'>");
		html += F("<h1>🚀 Gestión de Proyectos</h1>");
		html += F("<p>Selecciona y carga configuraciones de proyectos desde la "
				  "API</p>");

		html += F("<div>");
		html += F("<button class='btn btn-primary' onclick='loadProjects()'>🔄 "
				  "Actualizar Proyectos</button>");
		html += F("<button id='loadBtn' class='btn btn-success' "
				  "onclick='loadSelected()' disabled>⚙️ Cargar "
				  "Configuración</button>");
		html += F("</div>");

		html += F("<div id='status' style='display:none'></div>");
		html += F("<div id='projects' class='projects'></div>");
		html += F("</div>");

		// Add JavaScript
		html += F("<script>");
		html += F("let projects=[];let selectedId=null;");

		html += F("function showStatus(msg,type){");
		html += F("const s=document.getElementById('status');");
		html += F("s.textContent=msg;s.className='status "
				  "'+type;s.style.display='block';");
		html += F("if(type!=='loading')setTimeout(()=>s.style.display='none',"
				  "3000);}");

		html += F("async function loadProjects(){");
		html += F("showStatus('Cargando proyectos...','loading');");
		html += F("try{const r=await fetch('/api/projects');");
		html += F("if(!r.ok)throw new Error('Error HTTP: '+r.status);");
		html += F("const data=await r.json();");
		html += F("if(data.success&&data.data){projects=data.data;"
				  "displayProjects();");
		html += F("showStatus('✅ '+projects.length+' proyectos "
				  "cargados','success');}");
		html += F("else throw new Error('Respuesta inválida');}");
		html += F("catch(e){showStatus('❌ Error: '+e.message,'error');}}");

		html += F("function displayProjects(){");
		html += F("const c=document.getElementById('projects');");
		html += F("if(!projects.length){c.innerHTML='<p>No hay proyectos "
				  "disponibles</p>';return;}");
		html += F("c.innerHTML=projects.map(p=>`<div class='project' "
				  "onclick='selectProject(\"${p.id}\")'>");
		html += F("<div class='project-title'>${p.name}</div>");
		html += F("<div "
				  "class='project-description'>${p.description}</div></"
				  "div>`).join('');}");

		html += F("function selectProject(id){selectedId=id;");
		html += F("document.querySelectorAll('.project').forEach(p=>p."
				  "classList.remove('selected'));");
		html +=
			F("event.target.closest('.project').classList.add('selected');");
		html += F("document.getElementById('loadBtn').disabled=false;");
		html += F("const p=projects.find(pr=>pr.id===id);");
		html += F("showStatus('✅ Seleccionado: '+p.name,'success');}");

		html += F("async function loadSelected(){");
		html += F("if(!selectedId){showStatus('❌ No hay proyecto "
				  "seleccionado','error');return;}");
		html += F("const p=projects.find(pr=>pr.id===selectedId);");
		html +=
			F("showStatus('Cargando configuración de: '+p.name,'loading');");
		html += F("try{const r=await "
				  "fetch('/api/project/'+selectedId+'/load',{method:'POST'});");
		html += F("if(!r.ok)throw new Error('Error HTTP: '+r.status);");
		html += F("const result=await r.json();");
		html += F("if(result.success)showStatus('🎉 Configuración cargada: "
				  "'+p.name,'success');");
		html +=
			F("else throw new Error(result.message||'Error desconocido');}");
		html += F("catch(e){showStatus('❌ Error: '+e.message,'error');}}");

		html += F("</script></body></html>");

		request->send(200, "text/html", html);
	}

	/**
	 * HTTP request handler for the projects API endpoint
	 * @param request AsyncWebServerRequest object
	 */
	static void handleProjectsAPIRequest(AsyncWebServerRequest *request) {

		String jsonResponse = projectsClient.fetchProjectsJSON();
		DUMPLN("API request for projects list ", jsonResponse);
		if (jsonResponse.isEmpty()) {
			request->send(
				500, "application/json",
				"{\"success\":false,\"error\":\"Failed to fetch projects\"}");
			return;
		}

		request->send(200, "application/json", jsonResponse);
	}

	/**
	 * HTTP request handler for loading a project configuration
	 * @param request AsyncWebServerRequest object
	 */
	static void handleLoadProjectRequest(AsyncWebServerRequest *request) {
		String url = request->url();
		DUMPLN("Load project request: ", url);

		// Extract project ID from URL: /api/project/{id}/load
		/*
		int startIdx = url.indexOf("/api/project/") + 13;
		int endIdx = url.indexOf("/load");

		if (startIdx < 13 || endIdx == -1) {
			request->send(
				400, "application/json",
				"{\"success\":false,\"error\":\"Invalid project ID\"}");
			return;
		}

		String projectId = url.substring(startIdx, endIdx);
		Serial.print("Extracted project ID: ");
		Serial.println(projectId);

		// Fetch project configuration
		String configJson = projectsClient.fetchProjectConfigJSON(projectId);

		if (configJson.isEmpty()) {
			request->send(500, "application/json",
						  "{\"success\":false,\"error\":\"Failed to fetch "
						  "project configuration\"}");
			return;
		}

		// For now, just return success - in a full implementation, this would
		// apply the configuration
		Serial.println("Project configuration would be applied here");
		request->send(200, "application/json",
					  "{\"success\":true,\"message\":\"Configuration loaded "
					  "successfully\"}");
					  */
	}
};

#endif
