#ifndef _ESPALLON_PROJECTS_H
#define _ESPALLON_PROJECTS_H

#include "../../config.h"
#include "../../utils.h"
#include "../ProjectsAPIClient.h"
#include <ArduinoJson.h>
#include <ESPUI.h>

/**
 * Projects endpoint handler for ESPAllOn system
 * Provides web interface for project management and configuration loading
 */
class ESPAllOnProjectsRev {
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

#ifdef USE_LITTLEFS_MODE
		// Serve from LittleFS if available
		request->send(LittleFS, "/projects.html", "text/html");
#else
		// Serve embedded HTML
		auto *res = request->beginResponseStream("text/html");

		res->print(
			F("<!DOCTYPE html><html lang='es'><head><meta charset='UTF-8'>"));
		res->print(F("<meta name='viewport' content='width=device-width, "
					 "initial-scale=1.0'>"));
		res->print(F("<title>ESPAllOn - Proyectos</title>"));

		// Embed CSS
		res->print(F("<style>"));
		res->print(F("*{margin:0;padding:0;box-sizing:border-box}"));
		res->print(
			F("body{font-family:'Segoe UI',Tahoma,Geneva,Verdana,sans-serif;"));
		res->print(
			F("background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);"));
		res->print(F("min-height:100vh;padding:20px}"));
		res->print(
			F(".container{max-width:1200px;margin:0 auto;background:white;"));
		res->print(F("border-radius:15px;box-shadow:0 20px 40px "
					 "rgba(0,0,0,0.1);overflow:hidden}"));
		res->print(F(".header{background:linear-gradient(135deg,#2c3e50 "
					 "0%,#34495e 100%);"));
		res->print(F("color:white;padding:30px;text-align:center}"));
		res->print(F(".header h1{font-size:2.5em;margin-bottom:10px}"));
		res->print(F(".controls{padding:30px;background:#f8f9fa;border-bottom:"
					 "1px solid #e9ecef;"));
		res->print(F("display:flex;justify-content:space-between;align-items:"
					 "center;flex-wrap:wrap;gap:15px}"));
		res->print(F(".btn{padding:12px "
					 "24px;border:none;border-radius:8px;cursor:pointer;"));
		res->print(F("font-size:16px;font-weight:600;transition:all 0.3s "
					 "ease;text-decoration:none;display:inline-block}"));
		res->print(F(".btn-primary{background:linear-gradient(135deg,#3498db,#"
					 "2980b9);color:white}"));
		res->print(F(".btn-success{background:linear-gradient(135deg,#27ae60,#"
					 "229954);color:white}"));
		res->print(F(".btn:disabled{opacity:0.6;cursor:not-allowed}"));
		res->print(F(".status{padding:15px "
					 "30px;font-weight:600;text-align:center;border-bottom:1px "
					 "solid #e9ecef}"));
		res->print(F(".status.loading{background:#fff3cd;color:#856404}"));
		res->print(F(".status.success{background:#d4edda;color:#155724}"));
		res->print(F(".status.error{background:#f8d7da;color:#721c24}"));
		res->print(F(".projects-grid{padding:30px;display:grid;grid-template-"
					 "columns:repeat(auto-fill,minmax(350px,1fr));gap:20px}"));
		res->print(F(".project-card{border:2px solid "
					 "#e9ecef;border-radius:12px;padding:25px;"));
		res->print(
			F("transition:all 0.3s ease;cursor:pointer;background:white}"));
		res->print(F(".project-card:hover{border-color:#3498db;transform:"
					 "translateY(-5px);"));
		res->print(F("box-shadow:0 15px 30px rgba(0,0,0,0.1)}"));
		res->print(F(
			".project-card.selected{border-color:#27ae60;background:#f8fff9;"));
		res->print(F("box-shadow:0 8px 25px rgba(39,174,96,0.2)}"));
		res->print(F(".project-title{font-size:1.4em;font-weight:700;color:#"
					 "2c3e50;margin-bottom:10px}"));
		res->print(F(".project-description{color:#7f8c8d;margin-bottom:15px;"
					 "line-height:1.5}"));
		res->print(F(".project-meta{display:flex;justify-content:space-between;"
					 "align-items:center;"));
		res->print(F(
			"margin-top:15px;padding-top:15px;border-top:1px solid #ecf0f1}"));
		res->print(F(".project-status{padding:5px "
					 "12px;border-radius:20px;font-size:0.85em;"));
		res->print(F("font-weight:600;text-transform:uppercase}"));
		res->print(F(".status-active{background:#d4edda;color:#155724}"));
		res->print(F(".status-completed{background:#cce7ff;color:#004085}"));
		res->print(F(".status-inactive{background:#f8d7da;color:#721c24}"));
		res->print(F(
			".empty-state{text-align:center;padding:60px 30px;color:#7f8c8d}"));
		res->print(
			F(".loading-spinner{display:inline-block;width:20px;height:20px;"));
		res->print(F("border:3px solid #f3f3f3;border-top:3px solid "
					 "#3498db;border-radius:50%;"));
		res->print(F("animation:spin 1s linear infinite;margin-right:10px}"));
		res->print(F(
			"@keyframes "
			"spin{0%{transform:rotate(0deg)}100%{transform:rotate(360deg)}}"));
		res->print(
			F("@media "
			  "(max-width:768px){.container{margin:10px;border-radius:10px}"));
		res->print(F(".controls{padding:20px;flex-direction:column;align-items:"
					 "stretch}"));
		res->print(
			F(".projects-grid{grid-template-columns:1fr;padding:20px}}"));
		res->print(F("</style>"));

		res->print(F("</head><body>"));
		res->print(F("<div class='container'>"));
		res->print(F("<div class='header'>"));
		res->print(F("<h1>🚀 Gestión de Proyectos</h1>"));
		res->print(F("<p>Selecciona y carga configuraciones de proyectos desde "
					 "la API</p>"));
		res->print(F("</div>"));

		res->print(F("<div class='controls'>"));
		res->print(F("<button id='refreshBtn' class='btn btn-primary' "
					 "onclick='loadProjects()'>"));
		res->print(F(
			"<span id='refreshIcon'>🔄</span> Actualizar Proyectos</button>"));
		res->print(F("<button id='loadConfigBtn' class='btn btn-success' "
					 "onclick='loadSelectedProject()' disabled>"));
		res->print(
			F("<span id='loadIcon'>⚙️</span> Cargar Configuración</button>"));
		res->print(F("</div>"));

		res->print(
			F("<div id='status' class='status' style='display:none;'></div>"));
		res->print(F("<div id='projectsContainer' class='projects-grid'>"));
		res->print(F("<div class='empty-state'>"));
		res->print(F("<h3>👋 ¡Bienvenido!</h3>"));
		res->print(F("<p>Haz clic en \"Actualizar Proyectos\" para cargar los "
					 "proyectos disponibles desde la API.</p>"));
		res->print(F("</div></div></div>"));

		// Add JavaScript
		addProjectsJavaScript(res);

		res->print(F("</body></html>"));
		request->send(res);
#endif
	}

	/**
	 * HTTP request handler for the projects API endpoint
	 * @param request AsyncWebServerRequest object
	 */
	static void handleProjectsAPIRequest(AsyncWebServerRequest *request) {
		DUMPSLN("API request for projects list");

		String jsonResponse = projectsClient.fetchProjectsJSON();

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
		int startIdx = url.indexOf("/api/project/") + 13;
		int endIdx = url.indexOf("/load");

		if (startIdx < 13 || endIdx == -1) {
			request->send(
				400, "application/json",
				"{\"success\":false,\"error\":\"Invalid project ID\"}");
			return;
		}

		String projectId = url.substring(startIdx, endIdx);
		DUMPLN("Extracted project ID: ", projectId);

		// Fetch project configuration
		String configJson = projectsClient.fetchProjectConfigJSON(projectId);

		if (configJson.isEmpty()) {
			request->send(500, "application/json",
						  "{\"success\":false,\"error\":\"Failed to fetch "
						  "project configuration\"}");
			return;
		}

		// Parse and apply configuration
		JsonDocument doc;
		DeserializationError error = deserializeJson(doc, configJson);

		if (error) {
			DUMPLN("JSON parsing error: ", error.c_str());
			request->send(
				500, "application/json",
				"{\"success\":false,\"error\":\"Invalid JSON response\"}");
			return;
		}

		if (!doc["success"].as<bool>()) {
			request->send(
				500, "application/json",
				"{\"success\":false,\"error\":\"API returned error\"}");
			return;
		}

		// Apply configuration (simplified for now)
		bool success = applyProjectConfiguration(doc["data"]);

		if (success) {
			request->send(200, "application/json",
						  "{\"success\":true,\"message\":\"Configuration "
						  "loaded successfully\"}");
		} else {
			request->send(500, "application/json",
						  "{\"success\":false,\"error\":\"Failed to apply "
						  "configuration\"}");
		}
	}

	/**
	 * Apply project configuration to the system
	 * @param projectData JSON object containing project data
	 * @return True if configuration was applied successfully
	 */
	static bool applyProjectConfiguration(JsonObject projectData) {
		DUMPLN("Applying project configuration: ",
			   projectData["name"].as<String>());

		if (!projectData["config"].is<JsonObject>()) {
			DUMPSLN("No configuration object found");
			return false;
		}

		JsonObject config = projectData["config"];
		if (!config["config"].is<JsonArray>()) {
			DUMPSLN("No configuration array found");
			return false;
		}

		JsonArray configArray = config["config"];
		DUMPLN("Found configuration items: ", configArray.size());

		// For now, just log the configuration items
		// In a full implementation, this would integrate with ESPinner_Manager
		for (JsonObject item : configArray) {
			String id = item["ID"].as<String>();
			String mod = item["ESPinner_Mod"].as<String>();
			DUMPLN("Config item - ID: ", id);
			DUMPLN("Config item - Mod: ", mod);
		}

		return true;
	}

	/**
	 * Add JavaScript code to the response
	 * @param res Response stream
	 */
	static void addProjectsJavaScript(AsyncResponseStream *res) {
		res->print(F("<script>"));
		res->print(F("let projects=[];let selectedProjectId=null;"));

		res->print(F("function showStatus(message,type='loading'){"));
		res->print(F("const statusEl=document.getElementById('status');"));
		res->print(F("statusEl.textContent=message;statusEl.className=`status "
					 "${type}`;"));
		res->print(F("statusEl.style.display='block';"));
		res->print(F("if(type==='success'||type==='error'){"));
		res->print(
			F("setTimeout(()=>{statusEl.style.display='none';},5000);}}"));

		res->print(F("async function loadProjects(){"));
		res->print(
			F("const refreshBtn=document.getElementById('refreshBtn');"));
		res->print(
			F("const refreshIcon=document.getElementById('refreshIcon');"));
		res->print(F("refreshBtn.disabled=true;refreshIcon.innerHTML='<div "
					 "class=\"loading-spinner\"></div>';"));
		res->print(
			F("showStatus('Cargando proyectos desde la API...','loading');"));
		res->print(F("try{const response=await fetch('/api/projects');"));
		res->print(F("if(!response.ok)throw new Error(`HTTP error! status: "
					 "${response.status}`);"));
		res->print(F("const data=await response.json();"));
		res->print(F("if(data.success&&data.data){projects=data.data;"
					 "displayProjects(projects);"));
		res->print(F("showStatus(`✅ ${projects.length} proyectos cargados "
					 "exitosamente`,'success');}"));
		res->print(F("else{throw new Error('Respuesta de API inválida');}}"));
		res->print(
			F("catch(error){console.error('Error loading projects:',error);"));
		res->print(F("showStatus(`❌ Error al cargar proyectos: "
					 "${error.message}`,'error');displayEmptyState();}"));
		res->print(F("finally{refreshBtn.disabled=false;refreshIcon."
					 "textContent='🔄';}}"));

		res->print(F("function displayProjects(projectsList){"));
		res->print(
			F("const container=document.getElementById('projectsContainer');"));
		res->print(
			F("if(projectsList.length===0){displayEmptyState();return;}"));
		res->print(F("container.innerHTML=projectsList.map(project=>`"));
		res->print(F("<div class='project-card' "
					 "onclick='selectProject(\"${project.id}\")'>"));
		res->print(
			F("<div class='project-title'>${escapeHtml(project.name)}</div>"));
		res->print(F("<div "
					 "class='project-description'>${escapeHtml(project."
					 "description)}</div>"));
		res->print(F("<div class='project-meta'>"));
		res->print(F("<span class='project-status status-${project.status}'>"));
		res->print(
			F("${getStatusIcon(project.status)} ${project.status}</span>"));
		res->print(F("<span "
					 "class='project-date'>${formatDate(project.updated_at)}</"
					 "span></div></div>"));
		res->print(F("`).join('');}"));

		res->print(F("function displayEmptyState(){"));
		res->print(
			F("const container=document.getElementById('projectsContainer');"));
		res->print(F("container.innerHTML=`<div class='empty-state'>"));
		res->print(F("<h3>📭 No hay proyectos disponibles</h3>"));
		res->print(F("<p>No se encontraron proyectos en la API o hubo un error "
					 "al cargarlos.</p></div>`;}"));

		res->print(F(
			"function selectProject(projectId){selectedProjectId=projectId;"));
		res->print(
			F("document.querySelectorAll('.project-card').forEach(card=>{"));
		res->print(F("card.classList.remove('selected');});"));
		res->print(F("event.currentTarget.classList.add('selected');"));
		res->print(
			F("document.getElementById('loadConfigBtn').disabled=false;"));
		res->print(F("const project=projects.find(p=>p.id===projectId);"));
		res->print(F("showStatus(`✅ Proyecto seleccionado: "
					 "${project.name}`,'success');}"));

		res->print(F("async function loadSelectedProject(){"));
		res->print(F("if(!selectedProjectId){showStatus('❌ No hay proyecto "
					 "seleccionado','error');return;}"));
		res->print(
			F("const loadBtn=document.getElementById('loadConfigBtn');"));
		res->print(F("const loadIcon=document.getElementById('loadIcon');"));
		res->print(F("loadBtn.disabled=true;loadIcon.innerHTML='<div "
					 "class=\"loading-spinner\"></div>';"));
		res->print(
			F("const project=projects.find(p=>p.id===selectedProjectId);"));
		res->print(F("showStatus(`Cargando configuración de: "
					 "${project.name}...`,'loading');"));
		res->print(F("try{const response=await "
					 "fetch(`/api/project/${selectedProjectId}/load`,{"));
		res->print(
			F("method:'POST',headers:{'Content-Type':'application/json'}});"));
		res->print(F("if(!response.ok)throw new Error(`HTTP error! status: "
					 "${response.status}`);"));
		res->print(F("const result=await response.json();"));
		res->print(F("if(result.success){showStatus(`🎉 Configuración cargada "
					 "exitosamente: ${project.name}`,'success');}"));
		res->print(
			F("else{throw new Error(result.message||'Error desconocido');}}"));
		res->print(F("catch(error){console.error('Error loading project "
					 "config:',error);"));
		res->print(F("showStatus(`❌ Error al cargar configuración: "
					 "${error.message}`,'error');}"));
		res->print(
			F("finally{loadBtn.disabled=false;loadIcon.textContent='⚙️';}}"));

		res->print(F("function escapeHtml(text){const "
					 "div=document.createElement('div');"));
		res->print(F("div.textContent=text;return div.innerHTML;}"));

		res->print(F("function getStatusIcon(status){switch(status){"));
		res->print(
			F("case 'active':return '🟢';case 'completed':return '✅';"));
		res->print(F("case 'inactive':return '🔴';default:return '⚪';}}"));

		res->print(F("function formatDate(dateString){try{const date=new "
					 "Date(dateString);"));
		res->print(F("return "
					 "date.toLocaleDateString('es-ES',{year:'numeric',month:'"
					 "short',day:'numeric'});}"));
		res->print(F("catch{return 'Fecha inválida';}}"));

		res->print(F("</script>"));
	}
};

#endif
