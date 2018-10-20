#include "stdafx.h"
#include "StepFileInit.h"

StepFileInit::StepFileInit(DB_configuration* dbConfig)
{
	_dbConfig = dbConfig;
	_m = _dbConfig->getModel();
	_ma = _dbConfig->getMemAlloc();
}

StepFileInit::StepFileInit(Model* m, CMemoryAllocator* ma)
{
	_m = m;
	_ma = ma;
}


StepFileInit::~StepFileInit()
{
}

product_definition* StepFileInit::getProductDefinition(){
	return prodDef;
}
	
void StepFileInit::addMandatoryEntities(){

	//DECLARATION OF "MANDATORY ENTITY INSTANCES"
	//PRODUCT
	prodDef = createEntity(product_definition);

	product*							prod = createEntity(product);
	product_context*					prodCtxt = createEntity(product_context);
	application_protocol_definition*	appProtDef = createEntity(application_protocol_definition);
	application_context*				appCtxt = createEntity(application_context);

	product_definition_formation*		prodDefForm = createEntity(product_definition_formation);

	product_definition_context*			prodDefctxt = createEntity(product_definition_context);

	prodDefShape = createEntity(product_definition_shape);

	product_category_relationship*		prodCatRel = createEntity(product_category_relationship);
	product_category*					prodCat = createEntity(product_category);
	product_related_product_category*	prodRelProdCat = createEntity(product_related_product_category);

	//PERSON AND ORGANIZATION
	applied_person_and_organization_assignment* appliedPOassignDO = createEntity(applied_person_and_organization_assignment);
	applied_person_and_organization_assignment* appliedPOassignDS = createEntity(applied_person_and_organization_assignment);
	applied_person_and_organization_assignment* appliedPOassignCO = createEntity(applied_person_and_organization_assignment);
	applied_person_and_organization_assignment* appliedPOassignC = createEntity(applied_person_and_organization_assignment);

	person_and_organization* personAndOrganization = createEntity(person_and_organization);
	organization* organization_ = createEntity(organization);
	person* person_ = createEntity(person);

	person_and_organization_role* PO_RoleDO = createEntity(person_and_organization_role);
	person_and_organization_role* PO_RoleDS = createEntity(person_and_organization_role);
	person_and_organization_role* PO_RoleCO = createEntity(person_and_organization_role);
	person_and_organization_role* PO_RoleC = createEntity(person_and_organization_role);

	//SECURITY
	applied_security_classification_assignment* appliedSecClassAssign = createEntity(applied_security_classification_assignment);
	security_classification* secClass = createEntity(security_classification);
	security_classification_level* secClassLvl = createEntity(security_classification_level);

	//DATE AND TIME
	applied_date_and_time_assignment* appliedDateTimeAssign_class = createEntity(applied_date_and_time_assignment);
	applied_date_and_time_assignment* appliedDateTimeAssign_creation = createEntity(applied_date_and_time_assignment);
	date_time_role* dateTimeRole_class = createEntity(date_time_role);
	date_time_role* dateTimeRole_creation = createEntity(date_time_role);
	date_and_time* dateAndTime = createEntity(date_and_time);
	local_time* localTime = createEntity(local_time);
	calendar_date* calDate = createEntity(calendar_date);
	coordinated_universal_time_offset* coordUniTimeOff = createEntity(coordinated_universal_time_offset);

	//SET MANDATORY INSTANCES ATTRIBUTES
	//Product + Product Context
	prod->put_description(&analysis_description[0]);
	prod->put_id(&analysis_id[0]);
	prod->put_name(&analysis_name[0]);
	prod->put_frame_of_reference_element(prodCtxt);

	prodCtxt->put_name("");
	prodCtxt->put_discipline_type("analysis");
	prodCtxt->put_frame_of_reference(appCtxt);

	//Product Definition + Product Definition Context and Formation
	prodDefForm->put_id(&analysis_version[0]);
	prodDefForm->put_description(&analysis_version_description[0]);
	prodDefForm->put_of_product(prod);

	prodDef->put_id("analysis");
	prodDef->put_description("");
	prodDef->put_formation(prodDefForm);
	prodDef->put_frame_of_reference(prodDefctxt);

	prodDefctxt->put_name("");
	prodDefctxt->put_life_cycle_stage("ANALYSIS");
	prodDefctxt->put_frame_of_reference(appCtxt);

	//Product definition Shape
	prodDefShape->put_name((label)"");
	prodDefShape->put_description((text)"");
	prodDefShape->put_definition(prodDef);
	

	//Application Protocol Definition + Application context + id and description
	appProtDef->put_application(appCtxt);
	appProtDef->put_status("international standard");
	appProtDef->put_application_interpreted_model_schema_name("AP209_MULTIDISCIPLINARY_ANALYSIS_AND_DESIGN_MIM_LF");
	appProtDef->put_application_protocol_year(2014);

	appCtxt->put_application("AP209_MULTIDISCIPLINARY_ANALYSIS_AND_DESIGN_MIM_LF");

	

	//Product Category
	prodCatRel->put_description("");
	prodCatRel->put_name("");
	prodCatRel->put_category(prodCat);
	prodCatRel->put_sub_category(prodRelProdCat);

	prodCat->put_name("part");
	prodCat->put_description("");

	prodRelProdCat->put_description("");
	prodRelProdCat->put_name("detail");
	prodRelProdCat->put_products_element(prod);

	//Person and Organization
	appliedPOassignDO->put_assigned_person_and_organization(personAndOrganization);
	appliedPOassignDS->put_assigned_person_and_organization(personAndOrganization);
	appliedPOassignCO->put_assigned_person_and_organization(personAndOrganization);
	appliedPOassignC->put_assigned_person_and_organization(personAndOrganization);

	appliedPOassignDO->put_role(PO_RoleDO);
	appliedPOassignDS->put_role(PO_RoleDS);
	appliedPOassignCO->put_role(PO_RoleCO);
	appliedPOassignC->put_role(PO_RoleC);

	appliedPOassignDO->put_items_element(prod);
	appliedPOassignDS->put_items_element(prodDefForm);
	appliedPOassignCO->put_items_element(secClass);
	appliedPOassignC->put_items_element(prodDefForm);
	appliedPOassignC->put_items_element(prodDef);

	PO_RoleDO->put_name("design owner");
	PO_RoleDS->put_name("design supplier");
	PO_RoleCO->put_name("classification officer");
	PO_RoleC->put_name("creator");

	personAndOrganization->put_the_organization(organization_);
	personAndOrganization->put_the_person(person_);

	organization_->put_description(&organization_description[0]);
	organization_->put_id(&organization_id[0]);
	organization_->put_name(&organization_name[0]);

	person_->put_first_name(&person_first_name[0]);
	person_->put_last_name(&person_last_name[0]);
	person_->put_id(&person_id[0]);

	//Security
	appliedSecClassAssign->put_items_element(prodDefForm);
	appliedSecClassAssign->put_assigned_security_classification(secClass);

	secClass->put_name("");
	secClass->put_security_level(secClassLvl);
	secClass->put_purpose("");

	secClassLvl->put_name("unclassified");

	//date and time
	appliedDateTimeAssign_class->put_assigned_date_and_time(dateAndTime);
	appliedDateTimeAssign_class->put_role(dateTimeRole_class);
	appliedDateTimeAssign_class->put_items_element(secClass);

	appliedDateTimeAssign_creation->put_assigned_date_and_time(dateAndTime);
	appliedDateTimeAssign_creation->put_role(dateTimeRole_creation);
	appliedDateTimeAssign_creation->put_items_element(prodDef);

	dateTimeRole_class->put_name("classification date");
	dateTimeRole_creation->put_name("creation date");

	dateAndTime->put_date_component(calDate);
	dateAndTime->put_time_component(localTime);
	localTime->put_hour_component(creation_hour);
	localTime->put_zone(coordUniTimeOff);
	calDate->put_year_component(creation_year);
	calDate->put_month_component(creation_month);
	calDate->put_day_component(creation_day);
	coordUniTimeOff->put_hour_offset(time_offset_utc);
	coordUniTimeOff->put_sense(time_offset_ahead_or_behind);
}